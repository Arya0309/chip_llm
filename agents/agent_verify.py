#!/usr/bin/env python3
"""
LLM-based SystemC Verifier & Dispatcher  -- batch version with lazy init
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

MODEL_NAME = os.getenv("LLM_MODEL", DEFAULT_MODEL)

# lazy init：避免在匯入時就載模型
_LLM = None
def _get_llm() -> VLLMGenerator:
    global _LLM
    if _LLM is None:
        _LLM = VLLMGenerator(MODEL_NAME)
    return _LLM

_SYS_PROMPT = """
You are a senior SystemC verification engineer.
Return EXACTLY one line:
[REFINE] <none|dut|testbench|pipeline|dut,testbench|...> [/REFINE]
"""
_FILE_FMT = "** FILE: {name} **\n```cpp\n{code}\n```"
_REFINE_RE = re.compile(r"\[REFINE\]\s*([a-z,]+)\s*\[/REFINE\]", re.I)


def _build_prompt(srcs: Dict[str, str]) -> str:
    content = "\n\n".join(
        _FILE_FMT.format(name=n, code=srcs[n].strip())
        for n in (
            "Dut.cpp", "Dut.h",
            "Testbench.cpp", "Testbench.h",
            "SystemPipeline.cpp", "SystemPipeline.h",
        )
        if srcs.get(n)
    )
    msgs = [
        {"role": "system", "content": _SYS_PROMPT.strip()},
        {"role": "user", "content": content},
    ]
    return _get_llm().apply_chat_template(msgs, tokenize=False, add_generation_prompt=True)


def _parse_agents(out: str) -> List[str]:
    m = _REFINE_RE.search(out)
    if not m:
        return ["none"]
    lst = [s.strip().lower() for s in m.group(1).split(",") if s.strip()]
    return lst or ["none"]


def _append_issue_and_refine(
    agent_name: str,
    qprev: Path,
    qnext: Path,
    issue_msg: str,
    *,
    temperature: float,
    top_p: float,
    max_new_tokens: int,
):
    p_prompt = qprev / f"prompt_{agent_name}.json"
    if not p_prompt.exists():
        print(f"[WARN] prompt missing: {p_prompt}", file=sys.stderr)
        return

    msgs = json.loads(p_prompt.read_text("utf-8"))
    msgs.append({"role": "user", "content": issue_msg})

    # save new prompt
    out_prompt = qnext / p_prompt.name
    out_prompt.parent.mkdir(parents=True, exist_ok=True)
    out_prompt.write_text(json.dumps(msgs, ensure_ascii=False, indent=2), "utf-8")

    if agent_name == "dut":
        import agent_dut as ag
    elif agent_name == "testbench":
        import agent_tb as ag
    elif agent_name == "pipeline":
        import agent_pipe as ag
    else:
        return

    new_files = ag.refine(
        msgs,
        issue_msg,
        temperature=temperature,
        top_p=top_p,
        max_new_tokens=max_new_tokens,
    )
    for fn, code in new_files.items():
        (qnext / fn).write_text(code, "utf-8")


def _read(p: Path) -> str:
    return p.read_text("utf-8") if p.exists() else ""


def _collect_sources(qdir: Path) -> Dict[str, str]:
    return {n: _read(qdir / n) for n in (
        "Dut.cpp","Dut.h","Testbench.cpp","Testbench.h","SystemPipeline.cpp","SystemPipeline.h"
    )}


def _process_round(
    run_dir: Path,
    r: int,
    *,
    max_rounds: int,
    batch_size: int,
    temperature: float,
    top_p: float,
    max_new_tokens: int,
) -> bool:
    prev_r = run_dir / f"round_{r}"
    next_r = run_dir / f"round_{r+1}"
    if next_r.exists():
        shutil.rmtree(next_r)
    next_r.mkdir(parents=True, exist_ok=True)

    cases: list[dict] = []
    for qprev in prev_r.iterdir():
        if not qprev.is_dir():
            continue
        if qprev.name == "build":
            shutil.copytree(qprev, next_r / qprev.name, dirs_exist_ok=True)
            continue
        qnext = next_r / qprev.name
        shutil.copytree(qprev, qnext, dirs_exist_ok=True)
        cases.append(
            {"qprev": qprev, "qnext": qnext, "prompt": _build_prompt(_collect_sources(qprev))}
        )

    need_more = False
    if not cases:
        return need_more

    # 這裡才真正需要 LLM
    llm = _get_llm()

    for i in range(0, len(cases), batch_size):
        chunk = cases[i : i + batch_size]
        outs = llm.generate_batch(
            [c["prompt"] for c in chunk],
            max_new_tokens=max_new_tokens,
            temperature=temperature,
            top_p=top_p,
        )
        for c, raw in zip(chunk, outs):
            raw = raw.strip()
            c["qnext"].joinpath("verifier.txt").write_text(raw, "utf-8")
            agents = _parse_agents(raw)
            if agents == ["none"]:
                continue
            need_more = True
            issue_msg = (
                "[ISSUE] Verifier detected problems in "
                + ", ".join(agents)
                + ". Please regenerate accordingly."
            )
            for ag in agents:
                _append_issue_and_refine(
                    ag, c["qprev"], c["qnext"], issue_msg,
                    temperature=temperature, top_p=top_p, max_new_tokens=max_new_tokens,
                )

    if (r + 1) >= max_rounds and need_more:
        print("[INFO] Max rounds reached but issues remain.", file=sys.stderr)
    return need_more


def _args():
    ap = argparse.ArgumentParser()
    ap.add_argument("run_dir")
    ap.add_argument("--max_rounds", type=int, default=5)
    ap.add_argument("--batch_size", type=int, default=4)
    ap.add_argument("--temperature", type=float, default=0.3)
    ap.add_argument("--top_p", type=float, default=0.8)
    ap.add_argument("--max_new_tokens", type=int, default=4096)
    return ap.parse_args()


def main():
    a = _args()
    # 早退：max_rounds <= 1 直接結束，不初始化 LLM
    if a.max_rounds <= 1:
        print("[verifier] max_rounds<=1, skip.")
        return

    run_dir = Path(a.run_dir).resolve()
    if not (run_dir / "round_1").exists():
        sys.stderr.write("round_1 missing - run main_batch first.\n")
        sys.exit(1)

    for r in range(1, a.max_rounds):
        print(f"[verifier] round_{r} → round_{r+1}")
        more = _process_round(
            run_dir, r,
            max_rounds=a.max_rounds, batch_size=a.batch_size,
            temperature=a.temperature, top_p=a.top_p, max_new_tokens=a.max_new_tokens,
        )
        if not more:
            print("[verifier] All fixed, stop.")
            break


if __name__ == "__main__":
    main()
