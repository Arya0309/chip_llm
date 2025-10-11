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
[REFINE] <none|dut|testbench|pipeline|comma_separated> [/REFINE]
"""

_FILE_FMT = "** FILE: {name} **\n```cpp\n{code}\n```"
_REFINE_PAT = re.compile(r"\[REFINE\]\s*([a-z,]+)\s*\[/REFINE\]", re.I)
_OUTPUT_FMT = """** OUTPUTS: **
stdout: {stdout}
stderr: {stderr}
returncode: {returncode}
127 means timeout.
"""
# ------------------ 要再加入 compile 結果 ------------------


# ────────────────────────────────────────────────────────────────
# Prompt & 解析
# ────────────────────────────────────────────────────────────────
def _len_check(compile_results, max_len=4000):
    half = max_len // 2
    for k in ["stdout", "stderr"]:
        if len(compile_results[k]) > max_len:
            compile_results[k] = (
                compile_results[k][:half]
                + "\n...\n[TRUNCATED]\n...\n"
                + compile_results[k][-half:]
            )


def _build_prompt(srcs: Dict[str, str], compile_results) -> str:
    _len_check(compile_results, max_len=4000)
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


def _parse_refine(raw: str) -> List[str]:
    m = _REFINE_PAT.search(raw)
    if not m:
        return ["none"]
    agents = [s.strip().lower() for s in m.group(1).split(",") if s.strip()]
    return agents or ["none"]


# ────────────────────────────────────────────────────────────────
# 呼叫下游 agent
# ────────────────────────────────────────────────────────────────
def _append_issue_and_refine(
    agent_name: str,
    qdir_prev: Path,
    qdir_next: Path,
    issue_msg: str,
    *,
    temperature: float,
    top_p: float,
    max_new_tokens: int,
):
    prompt_path = qdir_prev / f"prompt_{agent_name}.json"
    if not prompt_path.exists():
        print(f"[WARN] prompt missing: {prompt_path}", file=sys.stderr)
        return

    messages = json.loads(prompt_path.read_text("utf-8"))
    messages.append({"role": "user", "content": issue_msg})

    # 存新 prompt（方便下次 round trace）
    prompt_out = qdir_next / prompt_path.name
    prompt_out.parent.mkdir(parents=True, exist_ok=True)
    prompt_out.write_text(json.dumps(messages, ensure_ascii=False, indent=2), "utf-8")

    # 呼叫對應 agent
    if agent_name == "dut":
        import agent_dut as ag
    elif agent_name == "testbench":
        import agent_tb as ag
    elif agent_name == "pipeline":
        import agent_pipe as ag
    else:
        return

    new_files, messages = ag.refine(
        messages,
        issue_msg,
        temperature=temperature,
        top_p=top_p,
        max_new_tokens=max_new_tokens,
    )
    for fname, code in new_files.items():
        (qdir_next / fname).write_text(code, "utf-8")

    (qdir_next / f"prompt_{agent_name}.json").write_text(
        json.dumps(messages, ensure_ascii=False, indent=2), "utf-8"
    )


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

    verifier_record = {}

    build_resuls = _build_process(prev_r)
    need_more = False
    for qdir_prev in prev_r.iterdir():
        if not qdir_prev.is_dir():
            continue
        if qdir_prev.name == "build":
            shutil.copytree(qdir_prev, next_r / qdir_prev.name, dirs_exist_ok=True)
            continue
        if "All test cases passed!" in build_resuls[qdir_prev.name]["stdout"]:
            continue

        qdir_next = next_r / qdir_prev.name
        shutil.copytree(qdir_prev, qdir_next, dirs_exist_ok=True)

        compile_results = build_resuls[qdir_prev.name]
        # stdout = compile_results["stdout"]
        # stderr = compile_results["stderr"]
        # returncode = compile_results["returncode"]  # <--- 127 代表 timeout
        # LLM 自行驗證是否要重作，並得到要重作的 agent 清單
        prompt = _build_prompt(_collect(qdir_prev), compile_results)

        # v--------------------------------------------------------------------------------------------------------- 蒐集這邊的prompt，這邊在做LLM自行驗證需要修改哪些部份
        raw = _llm.generate(prompt).strip()
        agents = _parse_refine(raw)

        verifier_record[qdir_prev.name] = {
            "prompt": prompt,
            "raw": raw,
            "agents": agents,
        }

        # 紀錄 verifier 回覆
        (qdir_next / "verifier.txt").write_text(raw, "utf-8")

        # 若無需重作，則跳過
        if agents == ["none"]:
            continue

        need_more = True
        issue_msg = (
            "[ISSUE] Verifier detected problems in "
            + ", ".join(agents)
            + ". Please regenerate accordingly."
        )
        # 呼叫下游 agent 進行重作，只會呼叫有列出的 agent
        # 可以改成根據compile的結果，再讓 LLM 決定要呼叫哪些 agent ---> 所以要再 build_prompt 加入 compile 結果，如下資訊
        for ag in agents:
            # v----------------------------------------------------------------------------------------------------- 這邊refine的部份可能每個問題要做的輪數不同
            _append_issue_and_refine(
                ag,
                qdir_prev,
                qdir_next,
                issue_msg,
                temperature=temperature,
                top_p=top_p,
                max_new_tokens=max_new_tokens,
            )

    # 紀錄本 round 的 verifier 結果
    with (next_r / "verifier_record.json").open("w", encoding="utf-8") as f:
        json.dump(verifier_record, f, ensure_ascii=False, indent=2)

    if (r + 1) >= max_rounds and need_more:
        print("[INFO] Max rounds reached but issues remain.", file=sys.stderr)
    return need_more


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
    return ap.parse_args()


def main():
    args = _parse_args()
    run_dir = Path(args.run_dir).resolve()
    if not (run_dir / "round_1").exists():
        sys.stderr.write("round_1 missing – run main_batch first.\n")
        sys.exit(1)

    for r in range(1, args.max_rounds):
        print(f"[verifier] round_{r} → round_{r+1}")
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
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
        "returncode": result.returncode,
    }


def _build_process(round_dir: Path) -> Dict[str, str]:
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
