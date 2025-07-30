#!/usr/bin/env python3
"""
main_batch.py — run an entire data_input.json in batches with automatic retry
"""
from __future__ import annotations
import argparse, json, os, sys, traceback
from pathlib import Path
from itertools import islice

from main import write_outputs

PROJ_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUT = PROJ_ROOT / ".log"
MAX_TRY = 5  # <— retry count for every batch call


# ---------- CLI ----------
def parse_args():
    ap = argparse.ArgumentParser()
    ap.add_argument("json_path")
    ap.add_argument("-o", "--out_dir", default=str(DEFAULT_OUT))
    ap.add_argument("--model", help="override env LLM_MODEL")
    ap.add_argument("--batch_size", type=int, default=8)
    return ap.parse_args()


# ---------- helpers ----------
def chunks(seq, n):
    it = iter(seq)
    while chunk := list(islice(it, n)):
        yield chunk


def retry(fn, tag: str):
    """Run fn() with automatic retry on ANY exception."""
    for attempt in range(1, MAX_TRY + 1):
        try:
            return fn()
        except Exception as e:
            print(f"[{tag}] attempt {attempt}/{MAX_TRY} failed: {e}", file=sys.stderr)
            if attempt == MAX_TRY:
                raise
            # optional: back‑off or change temperature here if desired


# ---------- Main ----------
def main():
    args = parse_args()
    if args.model:
        os.environ["LLM_MODEL"] = args.model
    import agent_func, agent_dut, agent_tb, agent_pipe  # Prevent default model loading

    items = json.loads(Path(args.json_path).read_text("utf-8"))
    out_root = Path(args.out_dir)
    out_root.mkdir(parents=True, exist_ok=True)

    # 1) metadata & raw code
    names, requirements, codes = [], [], []
    for it in items:
        names.append(it.get("new_name") or it["name"])
        requirements.append(it.get("requirement", ""))
        codes.append(it["code"])

    # 2) extract functions (batch + retry)
    func_sources = []
    for code_chunk in chunks(codes, args.batch_size):
        fn_lists = retry(
            lambda: agent_func.extract_functions_batch(code_chunk),
            tag="FUNC",
        )
        func_sources += ["\n\n".join(f["code"] for f in fl) for fl in fn_lists]

    # 3) generate DUT
    dut_results = []
    for src_chunk, req_chunk in zip(
        chunks(func_sources, args.batch_size),
        chunks(requirements, args.batch_size),
    ):
        dut_results += retry(
            lambda: agent_dut.generate_dut_batch(
                src_chunk, req_chunk, system_prompt=agent_dut._SYSTEM_PROMPT_V2
            ),
            tag="DUT",
        )
        print(f"Number of empty DUTs: {sum(1 for d in dut_results if not d)}")

    dut_cpp = [d["Dut.cpp"] for d in dut_results]
    dut_h = [d["Dut.h"] for d in dut_results]

    # 4) generate Testbench
    tb_results = []
    for cpp_chunk, h_chunk, req_chunk in zip(
        chunks(dut_cpp, args.batch_size),
        chunks(dut_h, args.batch_size),
        chunks(requirements, args.batch_size),
    ):
        tb_results += retry(
            lambda: agent_tb.generate_tb_batch(
                dut_cpp=cpp_chunk,
                dut_h=h_chunk,
                requirement=req_chunk,
                system_prompt=agent_tb._SYSTEM_PROMPT_V2,
            ),
            tag="TB",
        )

    tb_h_list = [tb["Testbench.h"] for tb in tb_results]

    # 5) generate Pipeline
    pipe_results = []
    for h_chunk, tb_h_chunk in zip(
        chunks(dut_h, args.batch_size),
        chunks(tb_h_list, args.batch_size),
    ):
        pipe_results += retry(
            lambda: agent_pipe.generate_pipeline_batch(
                h_chunk, tb_h_chunk, system_prompt=agent_pipe._SYSTEM_PROMPT_V2
            ),
            tag="PIPE",
        )

    # 6) write outputs
    for i, name in enumerate(names):
        write_outputs(
            dut_results[i], tb_results[i], pipe_results[i], out_root / name, name
        )
        # print(f"[OK] {name}")
    print(f"All outputs written to {out_root}")


if __name__ == "__main__":
    try:
        main()
    except Exception:
        # print full traceback once; individual retries already logged
        traceback.print_exc()
        sys.exit(1)
