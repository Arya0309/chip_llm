from __future__ import annotations

import argparse
import sys
import json
from pathlib import Path
from tempfile import NamedTemporaryFile
import os, contextlib
from typing import Union

import agents.agent_func as agent_func
import agents.agent_tb as agent_tb

# ---------- 全域常數 ----------
PROJECT_ROOT = Path(__file__).resolve().parents[1]  # /home/.../chip_llm
LOG_ROOT = PROJECT_ROOT / ".log_tb"  # /home/.../chip_llm/.log_tb
# --------------------------------


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(
        description="Convert C++ function(s) → SystemC Testbench.cpp via LLM agents"
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


def extract_entry(cpp_src: Union[str, Path], func_name: str | None):
    """
    cpp_src can be
      * Path  -> read file as before
      * str   -> raw C/C++ source code (e.g. from JSON)
    """
    # ---------- Path branch (unchanged) ----------
    if isinstance(cpp_src, Path):
        functions = agent_func.extract_functions(cpp_src)

    # ---------- Raw-string branch (new) ----------
    else:
        raw_code: str = cpp_src
        with NamedTemporaryFile("w+", suffix=".cpp", delete=False) as tmp:
            tmp.write(raw_code)
            tmp_path = Path(tmp.name)
        try:
            functions = agent_func.extract_functions(tmp_path)
        finally:
            # ensure the temp file is removed even if parsing fails
            with contextlib.suppress(FileNotFoundError):
                os.remove(tmp_path)

    # ---------- common post-processing ----------
    if not functions:
        raise RuntimeError("No non-main functions were extracted.")

    if func_name:
        entry = next((f for f in functions if f["name"] == func_name), None)
        if entry is None:
            fnames = ", ".join(f["name"] for f in functions)
            raise RuntimeError(f'Function "{func_name}" not found. Available: {fnames}')
        return entry

    # merge all by default
    return {"name": "combined", "code": "\n\n".join(f["code"] for f in functions)}


def write_outputs(tb_files: dict[str, str], out_dir: Path, custom_cpp: Path | None):
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
    print(f"[OK] Testbench.h  → {dir_path / 'Testbench.h'}")


# ---------- 主程式 ----------
def main() -> None:
    args = parse_args()
    src_path = Path(args.src_path).expanduser()

    if not src_path.exists():
        sys.exit(f"[Error] Path not found: {src_path}")

    # ------------------------------------------------------------
    # 0. JSON 模式（新）
    # ------------------------------------------------------------
    if src_path.suffix == ".json":
        entries = load_json_entries(src_path)
        for item in entries:
            print(f"--- Processing {item['name']} ---")
            func_code = item["code"]
            requirement = item.get("requirement", "")  # 可無
            try:
                entry = extract_entry(func_code, None)
                
                # tb_files = tb_agent.generate_tb(func_code=entry["code"], requirement=requirement)
                tb_files = agent_tb.generate_tb(
                    dut_cpp=item.get("Dut.cpp", ""),
                    dut_h=item.get("Dut.h", ""),
                    requirement=requirement,
                )
                # 以 new_name 優先作子資料夾；若沒有就用 name
                out_dir = LOG_ROOT / item.get("new_name", item["name"])
                write_outputs(tb_files, out_dir, None)
            except Exception as e:
                print(f"[Error] {item['name']}: {e}")
        return

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
                tb_files = agent_tb.generate_tb(entry["code"])
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
        tb_files = agent_tb.generate_tb(entry["code"])
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
        print(tb_files["Testbench.cpp"])


if __name__ == "__main__":
    main()
