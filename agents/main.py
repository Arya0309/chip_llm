#!/usr/bin/env python3
"""
Usage
-----
# 1. Single file (legacy behavior)
python main.py path/to/example.cpp                # extract every non-main function
python main.py path/to/example.cpp -f add         # extract only add()
python main.py path/to/example.cpp -o Dut.cpp     # write output to Dut.cpp

# 2. JSON batch input
python main.py path/to/data_input.json            # build DUTs from the JSON list
"""

from __future__ import annotations

import argparse
import sys
import json
from pathlib import Path

import func_agent
import dut_agent

# ---------- 全域常數 ----------
PROJECT_ROOT = Path(__file__).resolve().parents[1]  # /home/.../chip_llm
LOG_ROOT = PROJECT_ROOT / ".log"  # /home/.../chip_llm/.log
# --------------------------------


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(
        description="Convert C++ function(s) → SystemC Dut.cpp via LLM agents"
    )
    ap.add_argument(
        "src_path",
        help="Path to (i) a single .cpp, (ii) a directory with .cpp, **or** (iii) a data_input.json",
    )
    ap.add_argument(
        "-f",
        "--function",
        help="Target function name (file mode only; ignored when src_path is a directory)",
    )
    ap.add_argument(
        "-o",
        "--out",
        metavar="FILE",
        help="Custom output path (file mode only; ignored in directory mode)",
    )
    return ap.parse_args()


# ---------- 工具函式 ----------
def iter_cpp_files(p: Path) -> list[Path]:
    if p.is_file():
        return [p]
    return sorted(f for f in p.iterdir() if f.suffix == ".cpp" and f.is_file())


def load_json_entries(p: Path) -> list[dict]:
    data = json.loads(p.read_text(encoding="utf-8"))
    if not isinstance(data, list):
        raise ValueError("JSON must be a list of objects")
    return data


def extract_entry(cpp_path: Path, func_name: str | None) -> dict[str, str]:
    functions = func_agent.extract_functions(cpp_path)
    if not functions:
        raise RuntimeError("No non-main functions were extracted.")

    if func_name:
        entry = next((f for f in functions if f["name"] == func_name), None)
        if entry is None:
            fnames = ", ".join(f["name"] for f in functions)
            raise RuntimeError(f"Function “{func_name}” not found. Available: {fnames}")
        return entry

    # 預設行為：合併所有函式為一個 entry
    return {"name": "combined", "code": "\n\n".join(f["code"] for f in functions)}


def write_outputs(dut_files: dict[str, str], out_dir: Path, custom_cpp: Path | None):
    if custom_cpp:  # 單檔 + -o 指定
        dir_path = custom_cpp.parent
        cpp_path = custom_cpp
    else:  # (1) 單檔沒 -o；(2) 資料夾模式
        dir_path = out_dir
        cpp_path = dir_path / "Dut.cpp"

    dir_path.mkdir(parents=True, exist_ok=True)
    cpp_path.write_text(dut_files["Dut.cpp"], encoding="utf-8")
    (dir_path / "Dut.h").write_text(dut_files["Dut.h"], encoding="utf-8")

    print(f"[OK] Dut.cpp → {cpp_path}")
    print(f"[OK] Dut.h  → {dir_path / 'Dut.h'}")


# ---------- 主程式 ----------
def main() -> None:
    args = parse_args()
    src_path = Path(args.src_path).expanduser()

    if not src_path.exists():
        sys.exit(f"[Error] Path not found: {src_path}")

    # ------------------------------------------------------------
    #  A. JSON model
    # ------------------------------------------------------------
    if src_path.suffix == ".json":
        entries = load_json_entries(src_path)
        for item in entries:
            func_code = item["code"]
            requirement = item["requirement"]  # 可無
            dut_files = dut_agent.generate_dut(
                func_code, requirement=requirement  # 新增參數
            )
            # 以 new_name 優先作子資料夾；若沒有就用 name
            out_dir = LOG_ROOT / item.get("new_name", item["name"])
            write_outputs(dut_files, out_dir, None)
        return

    # ------------------------------------------------------------
    #  B. 單檔模式（維持舊行為）
    # ------------------------------------------------------------
    try:
        entry = extract_entry(src_path, args.function)
        dut_files = dut_agent.generate_dut(entry["code"])
    except Exception as e:
        sys.exit(f"[Error] {e}")

    # 決定輸出位置
    if args.out:
        custom_path = Path(args.out).expanduser()
        write_outputs(
            dut_files,
            custom_path.parent if custom_path.suffix else custom_path,
            custom_path if custom_path.suffix else None,
        )
    else:
        # 預設：印到 stdout
        print(dut_files["Dut.cpp"])


if __name__ == "__main__":
    main()
