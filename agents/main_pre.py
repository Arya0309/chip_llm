#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
main_pre.py — Pre‑synthesis pipeline helper (minimal artefacts)
---------------------------------------------------------------
1.  使用 *agent_func* 萃取純函式定義（沿用原 System Prompt）
2.  將所有函式合併成 **一個** `.cpp`
3.  送入 *agent_synth* 的 reflexion 迴圈
4.  僅輸出兩個檔案至 `--out_dir`：
      • `{cpp_name}.cpp`   – 合併後的可合成函式
      • `result.txt`       – 包含「最終 Reflexion 輸出」+「完整過程紀錄」
"""

from __future__ import annotations

import argparse
from pathlib import Path

import agent_func
import agent_synth


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------


def _build_cli() -> argparse.ArgumentParser:
    ap = argparse.ArgumentParser(
        prog="main_pre.py",
        description="Extract → combine → reflexion pipeline for Stratus synthesis.",
    )
    ap.add_argument(
        "src",
        help="Path to the original C/C++ translation unit (single *.cpp* file).",
    )
    ap.add_argument(
        "-o",
        "--out_dir",
        default=".log",
        help="Directory for generated artefacts.",
    )
    ap.add_argument(
        "--cpp_name",
        help="Override output cpp filename (without extension). Default = input stem.",
    )
    ap.add_argument("--max_iter", type=int, default=10)
    ap.add_argument("--temp_actor", type=float, default=0.3)
    ap.add_argument("--max_tokens", type=int, default=2048)
    return ap


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _write(path: Path, text: str) -> None:
    path.write_text(text, encoding="utf-8")


# ---------------------------------------------------------------------------
# Pipeline
# ---------------------------------------------------------------------------


def main() -> None:
    args = _build_cli().parse_args()
    src_path = Path(args.src).expanduser().resolve()
    out_dir = Path(args.out_dir).expanduser().resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    cpp_stem = args.cpp_name if args.cpp_name else src_path.stem
    cpp_path = out_dir / f"{cpp_stem}.cpp"
    result_path = out_dir / "result.txt"

    # 1. Extract functions
    print(f"[Info] Extracting functions from: {src_path}")
    functions = agent_func.extract_functions(
        src_path,
        system_prompt=agent_func._SYSTEM_PROMPT,  # 保持原始 prompt
    )
    if not functions:
        raise RuntimeError("agent_func returned no functions.")

    combined_code = "\n\n".join(f["code"] for f in functions)

    # 2. Run reflexion synthesis
    print("[Info] Running reflexion loop …")
    final_output, record = agent_synth.run_reflexion(
        combined_code,
        max_iter=args.max_iter,
        temp_actor=args.temp_actor,
        temp_env=0.3,
        max_tokens_actor=args.max_tokens,
        max_tokens_env=min(args.max_tokens, 2048),
    )

    # 3. Save result.txt (final + full record)
    record_text = "\n\n".join(record)
    result_content = (
        record_text + "\n\n===== FINAL REFLEXION OUTPUT =====\n" + final_output
    )
    _write(result_path, result_content)
    print(f"[Saved] result.txt        → {result_path}")
    _write(cpp_path, final_output)
    print(f"[Saved] {cpp_stem}.cpp     → {cpp_path}")
    print("\n[Done]")


if __name__ == "__main__":
    main()
