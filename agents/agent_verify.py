#!/usr/bin/env python3
# agents/agent_verifier.py
"""
LLM-based SystemC Verifier & Dispatcher  (支援溫度等參數傳遞)
"""

from __future__ import annotations
import argparse
import json
import os
import re
import shutil
import sys
from pathlib import Path
from typing import Dict, List

from utils import DEFAULT_MODEL, VLLMGenerator
from summary import SummaryAgent

# ────────────────────────────────────────────────────────────────
# 設定
# ────────────────────────────────────────────────────────────────
MODEL_NAME = os.getenv("LLM_MODEL", DEFAULT_MODEL)
_llm = VLLMGenerator(MODEL_NAME)


# ------------------ 要再加入 compile 結果 ------------------
_SYSTEM_PROMPT = """
You are a senior SystemC verification engineer.

Task
----
Read the six source files (Dut / Testbench / SystemPipeline, .h/.cpp) and the terminal outputs.
Return EXACTLY one line:
[REFINE] <dut|testbench|pipeline|comma_separated> [/REFINE]
"""

_FILE_FMT = "** FILE: {name} **\n```cpp\n{code}\n```"
_REFINE_PAT = re.compile(r"\[REFINE\]\s*([a-z,]+)\s*\[/REFINE\]", re.I)
_OUTPUT_FMT = """** OUTPUTS: **
stdout: {stdout}
stderr: {stderr}
returncode: {returncode}
127 means timeout.
"""

# v--------------需要完成的地方------------------v
_SUMMARY_SYSTEM_PROMPT = """{qname} {summary}"""


# ────────────────────────────────────────────────────────────────
# Prompt & 解析
# ────────────────────────────────────────────────────────────────
def _len_check(compile_results, max_len=2000):
    half = max_len // 2
    for k in ["stdout", "stderr"]:
        if len(compile_results[k]) > max_len:
            compile_results[k] = (
                compile_results[k][:half]
                + "\n...\n[TRUNCATED]\n...\n"
                + compile_results[k][-half:]
            )


def _build_prompt(srcs: Dict[str, str], compile_results: Dict) -> str:
    _len_check(compile_results, max_len=2000)
    blocks = "\n\n".join(
        _FILE_FMT.format(name=n, code=srcs[n].strip())
        for n in (
            "Dut.cpp",
            "Dut.h",
            "Testbench.cpp",
            "Testbench.h",
            "SystemPipeline.cpp",
            "SystemPipeline.h",
        )
        if srcs.get(n)
    )
    msgs = [
        {"role": "system", "content": _SYSTEM_PROMPT.strip()},
        {
            "role": "user",
            "content": blocks + "\n\n" + _OUTPUT_FMT.format(**compile_results),
        },
    ]
    return _llm.apply_chat_template(msgs, tokenize=False, add_generation_prompt=True)


def _build_summary_prompt(path: Path, qname: str, summary: Dict) -> str:
    history = json.loads(path.read_text("utf-8"))
    summary_msg = [
        {
            "role": "user",
            "content": _SUMMARY_SYSTEM_PROMPT.format(qname=qname, summary=summary),
        },
    ]
    return _llm.apply_chat_template(
        history + summary_msg,
        tokenize=False,
        add_generation_prompt=True,
    )


def _parse_refine(raw: str) -> List[str]:
    m = _REFINE_PAT.search(raw)
    if not m:
        return ["none"]
    agents = [s.strip().lower() for s in m.group(1).split(",") if s.strip()]
    return agents or ["none"]


# ────────────────────────────────────────────────────────────────
# 呼叫下游 agent
# ────────────────────────────────────────────────────────────────
def _multiagent_refine(
    map: Dict[str, List],
    qdir_prev: Path,
    qdir_next: Path,
    temperature: float,
    top_p: float,
    max_new_tokens: int,
):
    # map = {"prompt": [], "qname": [], "raw": [], "agents": []}
    qnames = map["qname"]
    agents = map["agents"]
    refine_map = {"qname": [], "agent": [], "parse_fn": [], "prompt": []}

    _ISSUE_MSG = """[ISSUE] Verifier detected problems in {agent}.\nPlease regenerate accordingly.
Please generate ** ONLY ** Dut.h.
"""
    _GENERATE_CPP = "Please generate ** ONLY ** Dut.cpp."

    for qname, ags in zip(qnames, agents):
        for ag in ags:
            refine_map["qname"].append(qname)
            refine_map["agent"].append(ag)
            if ag == "dut":
                import agent_dut as _ag
            elif ag == "testbench":
                import agent_tb as _ag
            elif ag == "pipeline":
                import agent_pipe as _ag
            else:
                raise ValueError(f"None/unknown agent: {ag}")

            parse_fn = _ag._parse_output
            refine_map["parse_fn"].append(parse_fn)

            prompt_path = qdir_prev / qname / f"prompt_{ag}.json"
            if not prompt_path.exists():
                raise FileNotFoundError(f"prompt missing: {prompt_path}")
            prompt = json.loads(prompt_path.read_text("utf-8"))
            prompt.append({"role": "user", "content": _ISSUE_MSG.format(agent=ag)})
            refine_map["prompt"].append(prompt)

    batch_size = 16
    remainder = len(refine_map["prompt"]) % batch_size
    if remainder != 0:
        rounds = len(refine_map["prompt"]) // batch_size + 1
    else:
        rounds = len(refine_map["prompt"]) // batch_size

    for i in range(rounds):
        batch_prompts = refine_map["prompt"][i * batch_size : (i + 1) * batch_size]
        parse_fns = refine_map["parse_fn"][i * batch_size : (i + 1) * batch_size]
        qnames = refine_map["qname"][i * batch_size : (i + 1) * batch_size]
        agents = refine_map["agent"][i * batch_size : (i + 1) * batch_size]
        # (1)
        input = []
        for prompt in batch_prompts:
            input.append(
                _llm.apply_chat_template(
                    prompt,
                    tokenize=False,
                    add_generation_prompt=True,
                )
            )

        # (2)
        responses_h = _llm.generate_batch(
            input,
            temperature=temperature,
            top_p=top_p,
            max_new_tokens=max_new_tokens,
        )
        for resp_h, prompt in zip(responses_h, batch_prompts):
            prompt.append({"role": "assistant", "content": resp_h})
            prompt.append({"role": "user", "content": _GENERATE_CPP})

        # (1)
        input = []
        for prompt in batch_prompts:
            input.append(
                _llm.apply_chat_template(
                    prompt,
                    tokenize=False,
                    add_generation_prompt=True,
                )
            )

        # (2)
        responses_cpp = _llm.generate_batch(
            input,
            temperature=temperature,
            top_p=top_p,
            max_new_tokens=max_new_tokens,
        )
        for resp_cpp, prompt in zip(responses_cpp, batch_prompts):
            prompt.append({"role": "assistant", "content": resp_cpp})

        # (3) parse & write
        for qname, ag, parse_fn, resp_h, resp_cpp, prompt in zip(
            qnames,
            agents,
            parse_fns,
            responses_h,
            responses_cpp,
            batch_prompts,
        ):
            try:
                results = parse_fn(resp_h, resp_cpp)
            except Exception as e:
                print(f"[multiagent_refine] parse error: {e}")
                results = {}

            for fname, code in results.items():
                (qdir_next / qname / fname).write_text(code, "utf-8")

            (qdir_next / qname / f"prompt_{ag}.json").write_text(
                json.dumps(prompt, ensure_ascii=False, indent=2), "utf-8"
            )


def _multiagent_summary_refine(
    map: Dict[str, List],
    qdir_prev: Path,
    qdir_next: Path,
    temperature: float,
    top_p: float,
    max_new_tokens: int,
): ...


# ────────────────────────────────────────────────────────────────
# round_i → round_{i+1}
# ────────────────────────────────────────────────────────────────
def _read(p: Path) -> str:
    return p.read_text("utf-8") if p.exists() else ""


def _collect(qdir: Path) -> Dict[str, str]:
    return {
        n: _read(qdir / n)
        for n in (
            "Dut.cpp",
            "Dut.h",
            "Testbench.cpp",
            "Testbench.h",
            "SystemPipeline.cpp",
            "SystemPipeline.h",
        )
    }


def _process_one_round(
    run_dir: Path,
    r: int,
    *,
    max_rounds: int,
    temperature: float,
    top_p: float,
    max_new_tokens: int,
) -> bool:
    prev_r = run_dir / f"round_{r}"
    next_r = run_dir / f"round_{r+1}"
    if next_r.exists():
        shutil.rmtree(next_r)
    next_r.mkdir(parents=True, exist_ok=True)

    # v------------------------------------------------------------------------------------------------------------------------------v
    # 這邊build完prompt應該要先看code之後做summary，也許決定要修改哪些檔案也可以在這邊做。甚至可以用majority來決定要改哪些檔案（temperature稍微高一點）。
    build_results = _build_process(prev_r)
    need_more = True
    # 先蒐集所有執行有問題的題目，之後讓llm用一個batch生成
    map = {"prompt": [], "qname": [], "raw": [], "agents": []}
    for qdir_prev in prev_r.iterdir():
        if not qdir_prev.is_dir():
            continue
        if qdir_prev.name == "build":
            shutil.copytree(qdir_prev, next_r / qdir_prev.name, dirs_exist_ok=True)
            continue
        if "All test cases passed!" in build_results[qdir_prev.name]["stdout"]:
            continue

        qdir_next = next_r / qdir_prev.name
        shutil.copytree(qdir_prev, qdir_next, dirs_exist_ok=True)

        compile_results = build_results[qdir_prev.name]
        prompt = _build_prompt(_collect(qdir_prev), compile_results)
        map["prompt"].append(prompt)
        map["qname"].append(qdir_prev.name)

    batch_size = 16
    remainder = len(map["prompt"]) % batch_size
    if remainder != 0:
        rounds = len(map["prompt"]) // batch_size + 1
    else:
        rounds = len(map["prompt"]) // batch_size

    for i in range(rounds):
        batch_prompts = map["prompt"][i * batch_size : (i + 1) * batch_size]
        qnames = map["qname"][i * batch_size : (i + 1) * batch_size]
        raws = _llm.generate_batch(
            batch_prompts,
            temperature=temperature,
            top_p=top_p,
            max_new_tokens=max_new_tokens,
        )
        map["raw"].extend(raws)
        for qname, raw in zip(qnames, raws):
            agent = _parse_refine(raw)
            map["agents"].append(agent)
            (next_r / f"{qname}/verifier.txt").write_text(raw, "utf-8")

    _multiagent_refine(map, prev_r, next_r, temperature, top_p, max_new_tokens)

    with (next_r / "verifier_record.json").open("w", encoding="utf-8") as f:
        json.dump(map, f, ensure_ascii=False, indent=2)

    if r + 1 >= max_rounds:
        print("[INFO] Max rounds reached but issues remain.", file=sys.stderr)
    return need_more


def _process_summary(
    run_dir: Path,
    r: int,
    *,
    max_rounds: int,
    temperature: float,
    top_p: float,
    max_new_tokens: int,
) -> bool:
    prev_r = run_dir / f"round_{r}"
    next_r = run_dir / f"round_{r+1}"
    if next_r.exists():
        shutil.rmtree(next_r)
    next_r.mkdir(parents=True, exist_ok=True)
    summaryAgent = SummaryAgent(
        model=_llm,
        temperature=temperature,
        top_p=top_p,
        max_new_tokens=max_new_tokens,
    )

    build_results = _build_process(prev_r)
    for qname in build_results.keys():
        qdir_prev = prev_r / qname
        if not qdir_prev.is_dir():
            continue
        if qdir_prev.name == "build":
            shutil.copytree(qdir_prev, next_r / qdir_prev.name, dirs_exist_ok=True)
            continue
        if "All test cases passed!" in build_results[qname]["stdout"]:
            continue

        qdir_next = next_r / qdir_prev.name
        shutil.copytree(qdir_prev, qdir_next, dirs_exist_ok=True)

        _len_check(build_results[qname], max_len=2000)
        summaryAgent.add_data({qname: build_results[qname]})

    # map = {
    #     "qname": {
    #         "agents": [],
    #         "summary": "",
    #     }
    # }
    summary_results = summaryAgent.summarize(prev_r, batch_size=16)
    (prev_r / "summary.json").write_text(
        json.dumps(summary_results, ensure_ascii=False, indent=2), "utf-8"
    )
    map = {"prompt": [], "qname": [], "agent": [], "refine_fn": []}
    for k, v in summary_results.items():
        for agent in v["agents"]:
            if agent == "dut":
                import agent_dut as ag
            elif agent == "testbench":
                import agent_tb as ag
            elif agent == "pipeline":
                import agent_pipe as ag
            else:
                raise ValueError(f"None/unknown agent: {agent}")

            parse_fn = ag._parse_output
            map["refine_fn"].append(parse_fn)

            prompt = _build_summary_prompt(
                (prev_r / k / f"prompt_{agent}.json"), k, v["summary"]
            )

            map["prompt"].append(prompt)
            map["qname"].append(k)
            map["agent"].append(agent)

    _multiagent_summary_refine(map, prev_r, next_r, temperature, top_p, max_new_tokens)

    # with (next_r / "verifier_record.json").open("w", encoding="utf-8") as f:
    #     json.dump(map, f, ensure_ascii=False, indent=2)

    # if r + 1 >= max_rounds:
    #     print("[INFO] Max rounds reached but issues remain.", file=sys.stderr)
    return False


# ────────────────────────────────────────────────────────────────
# CLI
# ────────────────────────────────────────────────────────────────
def _parse_args():
    ap = argparse.ArgumentParser()
    ap.add_argument("run_dir", help=".log/run_i path")
    ap.add_argument("--max_rounds", type=int, default=5)
    ap.add_argument("--temperature", type=float, default=0.3)
    ap.add_argument("--top_p", type=float, default=0.8)
    ap.add_argument("--max_new_tokens", type=int, default=4096)
    ap.add_argument("--summarize", type=bool, default=False)
    return ap.parse_args()


def main():
    args = _parse_args()
    run_dir = Path(args.run_dir).resolve()
    if not (run_dir / "round_1").exists():
        sys.stderr.write("round_1 missing – run main_batch first.\n")
        sys.exit(1)

    for r in range(1, args.max_rounds):
        print(f"[verifier] round_{r} → round_{r+1}")
        if args.summarize:
            more = _process_summary(
                run_dir,
                r,
                max_rounds=args.max_rounds,
                temperature=args.temperature,
                top_p=args.top_p,
                max_new_tokens=args.max_new_tokens,
            )
        else:
            more = _process_one_round(
                run_dir,
                r,
                max_rounds=args.max_rounds,
                temperature=args.temperature,
                top_p=args.top_p,
                max_new_tokens=args.max_new_tokens,
            )
        if not more:
            print("[verifier] All fixed, stop.")
            break


# ────────────────────────────────────────────────────────────────
# Compiler Refinement
# ────────────────────────────────────────────────────────────────

import shlex, subprocess


def _build(code_dir: Path) -> Dict:
    build_dir = code_dir / "build"

    if build_dir.exists():
        shutil.rmtree(build_dir, ignore_errors=True)
    build_dir.mkdir(parents=True, exist_ok=True)

    cmds = [
        "cmake ..",
        "cmake --build .",
        "cp ../testcases.txt .",
        "cp ../golden.txt .",
        "./test-dut",
    ]

    print(f"[INFO] Compiling in {build_dir}")
    for cmd in cmds:
        try:
            result = subprocess.run(
                shlex.split(cmd),
                cwd=build_dir,
                capture_output=True,
                text=True,
                timeout=10,
            )

        except FileNotFoundError as e:
            # 可執行檔/路徑不存在
            print(f"[ERROR] Failed")
            return {
                "stdout": result.stdout.strip(),
                "stderr": result.stderr.strip(),
                "returncode": 127,
            }

        except subprocess.TimeoutExpired as e:
            print(f"[ERROR] Timeout (10 seconds)")
            return {
                "stdout": "",
                "stderr": f"timed out. (10 seconds)",
                "returncode": -1,
            }

    shutil.rmtree(build_dir, ignore_errors=True)

    return {
        "qname": code_dir.name,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
        "returncode": result.returncode,
    }


def _build_process(round_dir: Path) -> Dict[str, Dict]:
    results = {}
    for code_dir in round_dir.iterdir():
        if not code_dir.is_dir():
            continue

        results[code_dir.name] = _build(code_dir)

    # 將結果寫入到 round_dir/build_results.json
    build_results_path = round_dir / "build_results.json"
    with build_results_path.open("w", encoding="utf-8") as f:
        json.dump(results, f, ensure_ascii=False, indent=2)
    return results


if __name__ == "__main__":
    main()
    # _compile_process(Path(".log/run_1/round_1"))
