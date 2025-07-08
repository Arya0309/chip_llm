#!/usr/bin/env python3
"""
python main.py path/to/example.cpp                # 取第一個非-main 函式
python main.py path/to/example.cpp -f add         # 指定函式名稱
python main.py path/to/example.cpp -o Dut.cpp     # 將輸出寫到檔案
"""

from __future__ import annotations
import argparse
import sys
from pathlib import Path

import func_agent
import dut_agent


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(
        description="Convert a C++ function → SystemC Dut.cpp via LLM agents"
    )
    ap.add_argument("cpp_file", help="Path to the C++ source file")
    ap.add_argument(
        "-f",
        "--function",
        help="Target function name (default: first non-main function found)",
    )
    ap.add_argument(
        "-o", "--out", metavar="FILE", help="Write Dut.cpp to FILE instead of stdout"
    )
    return ap.parse_args()


def main() -> None:
    args = parse_args()
    src_path = Path(args.cpp_file).expanduser()

    if not src_path.exists():
        sys.exit(f"[Error] File not found: {src_path}")

    try:
        functions = func_agent.extract_functions(src_path)
    except Exception as e:
        sys.exit(f"[func_agent] {e}")

    if not functions:
        sys.exit("[Error] No non-main functions were extracted.")

    if args.function:
        entry = next((f for f in functions if f["name"] == args.function), None)
        if entry is None:
            fnames = ", ".join(f["name"] for f in functions)
            sys.exit(
                f"[Error] Function “{args.function}” not found. Available: {fnames}"
            )
    else:
        entry = functions[0]

    try:
        dut_cpp = dut_agent.generate_dut(entry["code"])
    except Exception as e:
        sys.exit(f"[dut_agent] {e}")

    if args.out:
        out_path = Path(args.out).expanduser()
        out_path.write_text(dut_cpp, encoding="utf-8")
        print(f"[OK] Dut.cpp written to {out_path}")
    else:
        print(dut_cpp)


if __name__ == "__main__":
    main()
