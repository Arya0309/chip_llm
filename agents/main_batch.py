#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import sys
import traceback
from itertools import islice
from pathlib import Path
from typing import Dict, List, Optional, Set

from main import write_outputs

# ---------------------------------------------------------------------------
# Config
# ---------------------------------------------------------------------------
PROJ_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUT = PROJ_ROOT / ".log"
MAX_TRY_PER_ITEM = 3  # max attempts per item per stage

# ---------------------------------------------------------------------------
# CLI helpers
# ---------------------------------------------------------------------------


def parse_args():
    ap = argparse.ArgumentParser()
    ap.add_argument("json_path")
    ap.add_argument("-o", "--out_dir", default=str(DEFAULT_OUT))
    ap.add_argument("--model", help="override env LLM_MODEL")
    ap.add_argument("--batch_size", type=int, default=8)
    # ap.add_argument("--max_iter", type=int, default=10)
    ap.add_argument("--temperature", type=float, default=0.3)
    ap.add_argument("--max_new_tokens", type=int, default=4096)
    ap.add_argument("--round_id", type=int, default=1)
    ap.add_argument("--refine_round_number", type=int, default=5)
    return ap.parse_args()


# ---------------------------------------------------------------------------
# Utility helpers
# ---------------------------------------------------------------------------


def chunks(seq: List[int], n: int):
    """Yield successive *n*‑sized chunks of indices from *seq*."""
    it = iter(seq)
    while chunk := list(islice(it, n)):
        yield chunk


def assign_if_empty(slots: List, idx: int, value):
    """Only write *value* into *slots[idx]* if it is currently None."""
    if slots[idx] is None and value:
        slots[idx] = value
        return True  # newly filled
    return False


def verify_len(tag: str, in_len: int, out_len: int):
    if out_len != in_len:
        raise RuntimeError(f"[{tag}] output length {out_len} ≠ input {in_len}")


# ---------------------------------------------------------------------------
# Stage wrappers
# ---------------------------------------------------------------------------


def run_func_stage(codes: List[str], batch_size: int):
    import agent_func

    N = len(codes)
    results: List[Optional[str]] = [None] * N
    attempts = [0] * N
    pending: Set[int] = set(range(N))

    while pending:
        active = [i for i in pending if attempts[i] < MAX_TRY_PER_ITEM]
        if not active:
            break

        for idx_chunk in chunks(active, batch_size):
            code_chunk = [codes[i] for i in idx_chunk]
            try:
                outs = agent_func.extract_functions_batch(code_chunk, system_prompt=agent_func._SYSTEM_PROMPT_V2)
                verify_len("FUNC", len(idx_chunk), len(outs))
            except Exception as e:
                for i in idx_chunk:
                    attempts[i] += 1
                print(f"[FUNC] chunk failed → retry ({e})", file=sys.stderr)
                continue

            for k, funcs in enumerate(outs):
                idx = idx_chunk[k]
                if funcs:
                    filled = assign_if_empty(
                        results, idx, "\n\n".join(f["code"] for f in funcs)
                    )
                    if filled:
                        pending.discard(idx)
                else:
                    attempts[idx] += 1

    abandon = {i for i, r in enumerate(results) if r is None}
    return results, abandon


def run_dut_stage(
    func_src: List[Optional[str]], 
    reqs: List[str], 
    batch_size: int, 
    skip: Set[int],
    *,
    max_new_tokens: int,
    temperature: float,
):
    import agent_dut

    N = len(func_src)
    results: List[Optional[Dict[str, str]]] = [None] * N
    attempts = [0] * N
    pending: Set[int] = {i for i in range(N) if i not in skip}

    while pending:
        active = [i for i in pending if attempts[i] < MAX_TRY_PER_ITEM]
        if not active:
            break

        for idx_chunk in chunks(active, batch_size):
            src_chunk = [func_src[i] for i in idx_chunk]
            req_chunk = [reqs[i] for i in idx_chunk]
            try:
                outs = agent_dut.generate_dut_batch(
                    src_chunk, 
                    req_chunk, 
                    system_prompt=agent_dut._SYSTEM_PROMPT_V2,
                    max_new_tokens=max_new_tokens,
                    temperature=temperature,
                )
                verify_len("DUT", len(idx_chunk), len(outs))
            except Exception as e:
                for i in idx_chunk:
                    attempts[i] += 1
                print(f"[DUT] chunk failed → retry ({e})", file=sys.stderr)
                continue

            for k, out in enumerate(outs):
                idx = idx_chunk[k]
                if assign_if_empty(results, idx, out):
                    pending.discard(idx)
                else:
                    attempts[idx] += 1

    abandon = {i for i, r in enumerate(results) if r is None}
    return results, abandon


def run_tb_stage(
    dut_res: List[Optional[Dict[str, str]]],
    reqs: List[str],
    batch_size: int,
    skip: Set[int],
    *,
    max_new_tokens: int,
    temperature: float,
):
    import agent_tb

    N = len(dut_res)
    results: List[Optional[Dict[str, str]]] = [None] * N
    attempts = [0] * N
    pending: Set[int] = {
        i for i in range(N) if i not in skip and dut_res[i] is not None
    }

    while pending:
        active = [i for i in pending if attempts[i] < MAX_TRY_PER_ITEM]
        if not active:
            break

        for idx_chunk in chunks(active, batch_size):
            dut_cpp_chunk = [dut_res[i]["Dut.cpp"] for i in idx_chunk]
            dut_h_chunk = [dut_res[i]["Dut.h"] for i in idx_chunk]
            req_chunk = [reqs[i] for i in idx_chunk]
            try:
                outs = agent_tb.generate_tb_batch(
                    dut_cpp=dut_cpp_chunk,
                    dut_h=dut_h_chunk,
                    requirement=req_chunk,
                    system_prompt=agent_tb._SYSTEM_PROMPT_V2,
                    max_new_tokens=max_new_tokens,
                    temperature=temperature,
                )
                verify_len("TB", len(idx_chunk), len(outs))
            except Exception as e:
                for i in idx_chunk:
                    attempts[i] += 1
                print(f"[TB] chunk failed → retry ({e})", file=sys.stderr)
                continue

            for k, out in enumerate(outs):
                idx = idx_chunk[k]
                if assign_if_empty(results, idx, out):
                    pending.discard(idx)
                else:
                    attempts[idx] += 1

    abandon = {i for i, r in enumerate(results) if r is None}
    return results, abandon


def run_pipe_stage(
    dut_res: List[Optional[Dict[str, str]]],
    tb_res: List[Optional[Dict[str, str]]],
    batch_size: int,
    skip: Set[int],
    *,
    max_new_tokens: int,
    temperature: float,
):
    import agent_pipe

    N = len(dut_res)
    results: List[Optional[Dict[str, str]]] = [None] * N
    attempts = [0] * N
    pending: Set[int] = {
        i for i in range(N) if i not in skip and dut_res[i] and tb_res[i]
    }

    while pending:
        active = [i for i in pending if attempts[i] < MAX_TRY_PER_ITEM]
        if not active:
            break

        for idx_chunk in chunks(active, batch_size):
            dut_h_chunk = [dut_res[i]["Dut.h"] for i in idx_chunk]
            tb_h_chunk = [tb_res[i]["Testbench.h"] for i in idx_chunk]
            try:
                outs = agent_pipe.generate_pipeline_batch(
                    dut_h_chunk, 
                    tb_h_chunk, 
                    system_prompt=agent_pipe._SYSTEM_PROMPT_V2,
                    max_new_tokens=max_new_tokens,
                    temperature=temperature
                )
                verify_len("PIPE", len(idx_chunk), len(outs))
            except Exception as e:
                for i in idx_chunk:
                    attempts[i] += 1
                print(f"[PIPE] chunk failed → retry ({e})", file=sys.stderr)
                continue

            for k, out in enumerate(outs):
                idx = idx_chunk[k]
                if assign_if_empty(results, idx, out):
                    pending.discard(idx)
                else:
                    attempts[idx] += 1

    abandon = {i for i, r in enumerate(results) if r is None}
    return results, abandon

# ---------------------------------------------------------------------------
# Prompt dumping helpers, TODO: dump的東西要再修正，目前這樣吃不到few shot範例，所以refine產生的東西不會follow我們想要的格式
# ---------------------------------------------------------------------------


def dump_prompt_dut(
    out_dir: Path,
    system_prompt: str,
    func_src: str,
    requirement: str,
):
    msg = [
        {"role": "system", "content": system_prompt},
        {
            "role": "user",
            "content": f"### C++ Functions\n```cpp\n{func_src}\n```\n\n### Requirement\n{requirement}",
        },
    ]
    (out_dir / "prompt_dut.json").write_text(
        json.dumps(msg, ensure_ascii=False, indent=2), encoding="utf-8"
    )


def dump_prompt_tb(
    out_dir: Path,
    system_prompt: str,
    dut_cpp: str,
    dut_h: str,
    requirement: str,
):
    msg = [
        {"role": "system", "content": system_prompt},
        {
            "role": "user",
            "content": (
                f"### Dut.cpp\n```cpp\n{dut_cpp}\n```\n\n"
                f"### Dut.h\n```cpp\n{dut_h}\n```\n\n"
                f"### Requirement\n{requirement}"
            ),
        },
    ]
    (out_dir / "prompt_testbench.json").write_text(
        json.dumps(msg, ensure_ascii=False, indent=2), encoding="utf-8"
    )


def dump_prompt_pipe(
    out_dir: Path,
    system_prompt: str,
    dut_h: str,
    tb_h: str,
):
    msg = [
        {"role": "system", "content": system_prompt},
        {
            "role": "user",
            "content": (
                f"### Dut.h\n```cpp\n{dut_h}\n```\n\n"
                f"### Testbench.h\n```cpp\n{tb_h}\n```"
            ),
        },
    ]
    (out_dir / "prompt_pipeline.json").write_text(
        json.dumps(msg, ensure_ascii=False, indent=2), encoding="utf-8"
    )


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main():
    args = parse_args()
    if args.model:
        os.environ["LLM_MODEL"] = args.model

    import agent_func, agent_dut, agent_tb, agent_pipe  # noqa: F401  heavy libs

    items = json.loads(Path(args.json_path).read_text("utf-8"))
    names: List[str] = []
    reqs: List[str] = []
    codes: List[str] = []
    for it in items:
        names.append(it.get("new_name") or it["name"])
        reqs.append(it.get("requirement", ""))
        codes.append(it["code"])

    out_root = Path(args.out_dir)
    out_root.mkdir(parents=True, exist_ok=True)

    # Stage 1 — FUNC
    func_src, func_abdn = run_func_stage(codes, args.batch_size)

    # Stage 2 — DUT
    dut_res, dut_abdn = run_dut_stage(
        func_src, reqs, args.batch_size, func_abdn,
        max_new_tokens=args.max_new_tokens, temperature=args.temperature,
        )

    # Stage 3 — TB
    tb_res, tb_abdn = run_tb_stage(
        dut_res, reqs, args.batch_size, func_abdn | dut_abdn,
        max_new_tokens=args.max_new_tokens, temperature=args.temperature,
        )

    # Stage 4 — PIPE
    pipe_res, pipe_abdn = run_pipe_stage(
        dut_res, tb_res, args.batch_size, func_abdn | dut_abdn | tb_abdn,
        max_new_tokens=args.max_new_tokens, temperature=args.temperature,
    )

    # Write outputs
    abandon_all = func_abdn | dut_abdn | tb_abdn | pipe_abdn
    for i, name in enumerate(names):
        out_dir = out_root / name
        out_dir.mkdir(parents=True, exist_ok=True)
        if i in abandon_all:
            print(f"[ABANDON] {name}")
            continue
        write_outputs(dut_res[i], tb_res[i], pipe_res[i], out_dir, name)

        # --- Dump prompts for verifier -------------------------------------
        dump_prompt_dut(
            out_dir,
            agent_dut._SYSTEM_PROMPT_V2,
            func_src[i],
            reqs[i],
        )
        dump_prompt_tb(
            out_dir,
            agent_tb._SYSTEM_PROMPT_V2,
            dut_res[i]["Dut.cpp"],
            dut_res[i]["Dut.h"],
            reqs[i],
        )
        dump_prompt_pipe(
            out_dir,
            agent_pipe._SYSTEM_PROMPT_V2,
            dut_res[i]["Dut.h"],
            tb_res[i]["Testbench.h"],
        )

    # meta.json (optional) — 包含 round 與 refine 次數設定
    meta = {
        "round_id": args.round_id,
        "refine_round_number": args.refine_round_number,
    }
    (out_root / "meta.json").write_text(
        json.dumps(meta, ensure_ascii=False, indent=2), encoding="utf-8"
    )

    print(f"Done. Files in {out_root}")

# ---------------------------------------------------------------------------
if __name__ == "__main__":
    try:
        main()
    except Exception:
        traceback.print_exc()
        sys.exit(1)
