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
    func_src: List[Optional[str]], reqs: List[str], batch_size: int, skip: Set[int]
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
                    src_chunk, req_chunk, system_prompt=agent_dut._SYSTEM_PROMPT_V2
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
                    dut_h_chunk, tb_h_chunk, system_prompt=agent_pipe._SYSTEM_PROMPT_V2
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
    dut_res, dut_abdn = run_dut_stage(func_src, reqs, args.batch_size, func_abdn)

    # Stage 3 — TB
    tb_res, tb_abdn = run_tb_stage(dut_res, reqs, args.batch_size, func_abdn | dut_abdn)

    # Stage 4 — PIPE
    pipe_res, pipe_abdn = run_pipe_stage(
        dut_res, tb_res, args.batch_size, func_abdn | dut_abdn | tb_abdn
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

    print(f"Done. Files in {out_root}")


# ---------------------------------------------------------------------------
if __name__ == "__main__":
    try:
        main()
    except Exception:
        traceback.print_exc()
        sys.exit(1)
