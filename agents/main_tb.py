#!/usr/bin/env python3
"""
Usage
-----
# 1. 單檔：維持舊行為
python main.py path/to/example.cpp                # 提取所有非-main 函式
python main.py path/to/example.cpp -f add         # 只提取 add()
python main.py path/to/example.cpp -o tb.cpp     # 輸出到指定檔名

# 2. 資料夾：掃描該資料夾下每個 .cpp
python main.py path/to/dir_with_cpp/
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

import func_agent
import tb_agent

# ---------- 全域常數 ----------
PROJECT_ROOT = Path(__file__).resolve().parents[1]  # /home/.../chip_llm
LOG_ROOT = PROJECT_ROOT / ".log_tb"  # /home/.../chip_llm/.log
# --------------------------------


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(
        description="Convert C++ function(s) → SystemC tb.cpp via LLM agents"
    )
    ap.add_argument(
        "src_path", help="Single .cpp file **or** a directory that contains .cpp files"
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
    """回傳所有要處理的 .cpp 檔案（不遞迴）。"""
    if p.is_file():
        return [p]
    return sorted(f for f in p.iterdir() if f.suffix == ".cpp" and f.is_file())


def extract_entry(cpp_path: Path, func_name: str | None) -> dict[str, str]:
    """呼叫 func_agent，回傳 {'name': ..., 'code': ...}。可額外合併函式。"""
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


def write_outputs(tb_files: dict[str, str], out_dir: Path, custom_cpp: Path | None):
    """將 tb.cpp / tb.h 寫到目標路徑。custom_cpp 只在單檔模式才會帶值。"""
    if custom_cpp:  # 單檔 + -o 指定
        dir_path = custom_cpp.parent
        cpp_path = custom_cpp
    else:  # (1) 單檔沒 -o；(2) 資料夾模式
        dir_path = out_dir
        cpp_path = dir_path / "Testbench.cpp"

    dir_path.mkdir(parents=True, exist_ok=True)
    cpp_path.write_text(tb_files["Testbench.cpp"], encoding="utf-8")
    (dir_path / "Testbench.h").write_text(tb_files["Testbench.h"], encoding="utf-8")

    print(f"[OK] Testbench.cpp → {cpp_path}")
    print(f"[OK] Testbench.h  → {dir_path / 'tb.h'}")


# ---------- 主程式 ----------
def main() -> None:
    args = parse_args()
    src_path = Path(args.src_path).expanduser()

    if not src_path.exists():
        sys.exit(f"[Error] Path not found: {src_path}")

    # ------------------------------------------------------------
    #  A. 資料夾模式
    # ------------------------------------------------------------
    if src_path.is_dir():
        if args.out or args.function:
            print("[Warn] -o / -f 參數在資料夾模式下會被忽略。")

        cpp_files = iter_cpp_files(src_path)
        if not cpp_files:
            sys.exit("[Error] Directory contains no .cpp files.")

        print(f"[Info] Found {len(cpp_files)} .cpp files in {src_path}\n")

        for cpp in cpp_files:
            print(f"--- Processing {cpp.name} ---")
            try:
                entry = extract_entry(cpp, None)  # 合併函式
                tb_files = tb_agent.generate_tb(entry["code"])
                out_dir = LOG_ROOT / cpp.stem  # .log/<檔名去副檔>
                write_outputs(tb_files, out_dir, None)
            except Exception as e:
                print(f"[Error] {cpp.name}: {e}")
            print()

        return  # 全部跑完就結束

    # ------------------------------------------------------------
    #  B. 單檔模式（維持舊行為）
    # ------------------------------------------------------------
    try:
        entry = extract_entry(src_path, args.function)
        tb_files = tb_agent.generate_tb(entry["code"])
    except Exception as e:
        sys.exit(f"[Error] {e}")

    # 決定輸出位置
    if args.out:
        custom_path = Path(args.out).expanduser()
        write_outputs(
            tb_files,
            custom_path.parent if custom_path.suffix else custom_path,
            custom_path if custom_path.suffix else None,
        )
    else:
        # 預設：印到 stdout
        print(tb_files["tb.cpp"])


if __name__ == "__main__":
    main()
