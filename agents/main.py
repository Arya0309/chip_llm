#!/usr/bin/env python3
"""

Usage
-----
1. **Single C++ file** (retains legacy behaviour)
   python main.py path/to/example.cpp                      # extract every non-main function
   python main.py path/to/example.cpp -f add               # only extract add()
   python main.py path/to/example.cpp -o /tmp/out_dir/     # write outputs to custom directory

2. **Directory** – process every `.cpp` inside the folder
   python main.py path/to/dir_with_cpp/

3. **JSON batch mode** – list of translation units as raw strings
   python main.py path/to/batch.json

   Each list element may contain:
     • "name"        – original identifier (required)
     • "code"        – raw C/C++ source as a string (required)
     • "new_name"    – override output folder name (optional)
     • "requirement" – extra hint forwarded to LLM agents (optional)

Outputs
-------
For every input unit the script creates **six files** —
   Dut.cpp, Dut.h,
   Testbench.cpp, Testbench.h,
   SystemPipeline.cpp, SystemPipeline.h
which are placed under:
   <project_root>/.log/<input_stem>/

Changes in this version (2025-07-21)
------------------------------------
1. **pipe_agent integration** – after `dut_agent` + `tb_agent`, we now call
   `pipe_agent.generate_pipeline()` to obtain `SystemPipeline.cpp/h`.
2. **Unified `write_outputs()`** – now writes all six files into a single output folder.
3. **Directory & JSON modes upgraded** – both now generate Testbench and Pipeline as well.
"""

from __future__ import annotations

import argparse
import os
import sys
import json
import contextlib
from tempfile import NamedTemporaryFile
from typing import Union
from pathlib import Path

early = argparse.ArgumentParser(add_help=False)
early.add_argument("--model", dest="llm_model")
early_args, _ = early.parse_known_args()
if early_args.llm_model:
    os.environ["LLM_MODEL"] = early_args.llm_model

from pathlib import Path
import utils
import agent_dut
import agent_func
import agent_tb
import agent_pipe

# ---------- Constants ----------
PROJECT_ROOT = Path(__file__).resolve().parents[1]  # /home/.../chip_llm
LOG_ROOT = PROJECT_ROOT / ".log"
# --------------------------------


# ----------------- CLI -----------------
def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(
        parents=[early],
        description="Convert C++ ⇒ SystemC via LLM agents",
    )
    ap.add_argument(
        "src_path",
        help="Path to (i) a single .cpp, (ii) a directory of .cpp, or (iii) data_input.json",
    )
    ap.add_argument(
        "-f",
        "--function",
        help="Target function name (only when src_path is a single .cpp)",
    )
    ap.add_argument(
        "-o",
        "--out",
        metavar="FILE",
        help="Custom output path (single‑file mode only)",
    )
    return ap.parse_args()


# --------------- Helpers ----------------


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
    """Return {name, code} where *code* contains one or more non‑main functions."""
    # ------ Path branch ------
    if isinstance(cpp_src, Path):
        functions = agent_func.extract_functions(cpp_src)
    # ------ Raw‑string branch ------
    else:
        raw_code: str = cpp_src
        with NamedTemporaryFile("w+", suffix=".cpp", delete=False) as tmp:
            tmp.write(raw_code)
            tmp_path = Path(tmp.name)
        try:
            functions = agent_func.extract_functions(tmp_path)
        finally:
            with contextlib.suppress(FileNotFoundError):
                os.remove(tmp_path)

    if not functions:
        raise RuntimeError("No non‑main functions were extracted.")

    if func_name:
        entry = next((f for f in functions if f["name"] == func_name), None)
        if entry is None:
            available = ", ".join(f["name"] for f in functions)
            raise RuntimeError(
                f'Function "{func_name}" not found. Available: {available}'
            )
        return entry

    # merge all by default
    return {"name": "combined", "code": "\n\n".join(f["code"] for f in functions)}


# ---------- I/O ----------


def write_outputs(
    dut_files: dict[str, str],
    tb_files: dict[str, str],
    pipe_files: dict[str, str],
    out_dir: Path,
    task_name: str | None = None,
):
    """Write Dut.*, Testbench.*, SystemPipeline.* into *out_dir*."""

    # DUT
    (out_dir / "Dut.cpp").write_text(dut_files["Dut.cpp"], encoding="utf-8")
    (out_dir / "Dut.h").write_text(dut_files["Dut.h"], encoding="utf-8")
    # Testbench
    (out_dir / "Testbench.cpp").write_text(tb_files["Testbench.cpp"], encoding="utf-8")
    (out_dir / "Testbench.h").write_text(tb_files["Testbench.h"], encoding="utf-8")
    # Pipeline
    (out_dir / "SystemPipeline.cpp").write_text(
        pipe_files["SystemPipeline.cpp"], encoding="utf-8"
    )
    (out_dir / "SystemPipeline.h").write_text(
        pipe_files["SystemPipeline.h"], encoding="utf-8"
    )
    (out_dir / "CMakeLists.txt").write_text(utils.get_cmake_list(), encoding="utf-8")
    (out_dir / "main.cpp").write_text(utils.get_main_cpp(), encoding="utf-8")
    (out_dir / "testcases.txt").write_text(
        utils.get_testcases(task_name or "default"), encoding="utf-8"
    )
    (out_dir / "golden.txt").write_text(
        utils.get_golden(task_name or "default"), encoding="utf-8"
    )
    # (out_dir / "testcases.txt").write_text(
    #     utils.get_testcases_and_golden(task_name or "default"), encoding="utf-8"
    # )

    print(f"[OK] All files written to {out_dir}")


# ----------------- Main -----------------


def main() -> None:
    args = parse_args()
    src_path = Path(args.src_path).expanduser()

    if not src_path.exists():
        sys.exit(f"[Error] Path not found: {src_path}")

    # =============================================================
    # 0. JSON mode
    # =============================================================
    if src_path.suffix == ".json":
        entries = load_json_entries(src_path)
        for item in entries:
            print(f"--- Processing {item['name']} ---")
            requirement = item.get("requirement", "")

            out_dir = LOG_ROOT / item["name"]
            out_dir.mkdir(parents=True, exist_ok=True)

            def retry(fn, max_try=5, *, name):
                for i in range(1, max_try + 1):
                    try:
                        return fn()
                    except Exception as e:
                        print(f"[{name}] attempt {i}/{max_try} failed: {e}")

            entry = retry(
                lambda: extract_entry(item["code"], item.get("function")),
                name="extract_entry",
            )

            dut_files = retry(
                lambda: agent_dut.generate_dut(entry["code"], requirement), name="DUT"
            )

            tb_files = retry(
                lambda: agent_tb.generate_tb(
                    dut_cpp=dut_files["Dut.cpp"],
                    dut_h=dut_files["Dut.h"],
                    requirement=requirement,
                ),
                name="Testbench",
            )

            pipe_files = retry(
                lambda: agent_pipe.generate_pipeline(
                    dut_files["Dut.h"], tb_files["Testbench.h"]
                ),
                name="Pipeline",
            )
            try:
                write_outputs(dut_files, tb_files, pipe_files, out_dir, item["name"])
            except Exception as e:
                print(f"[Error] {item['name']}: {e}")

            # try:
            #     # 1) extract functions
            #     entry = extract_entry(item["code"], None)

            #     # 2) DUT
            #     dut_files = agent_dut.generate_dut(entry["code"], requirement)
            #     # 3) Testbench
            #     tb_files = agent_tb.generate_tb(
            #         dut_cpp=dut_files["Dut.cpp"],
            #         dut_h=dut_files["Dut.h"],
            #         requirement=requirement,
            #     )
            #     # 4) Pipeline
            #     pipe_files = agent_pipe.generate_pipeline(
            #         dut_files["Dut.h"], tb_files["Testbench.h"]
            #     )

            #     # 5) Output dir
            #     out_dir = LOG_ROOT / item["name"]
            #     write_outputs(dut_files, tb_files, pipe_files, out_dir, item["name"])
            # except Exception as e:
            #     print(f"[Error] {item['name']}: {e}")
        return

    # =============================================================
    # A. Directory mode
    # =============================================================
    if src_path.is_dir():
        if args.out or args.function:
            print("[Warn] -o / -f parameters are ignored in directory mode.")

        cpp_files = iter_cpp_files(src_path)
        if not cpp_files:
            sys.exit("[Error] Directory contains no .cpp files.")

        print(f"[Info] Found {len(cpp_files)} .cpp files in {src_path}\n")

        for cpp in cpp_files:
            print(f"--- Processing {cpp.name} ---")
            try:
                entry = extract_entry(cpp, None)
                dut_files = agent_dut.generate_dut(entry["code"])
                tb_files = agent_tb.generate_tb(
                    dut_cpp=dut_files["Dut.cpp"], dut_h=dut_files["Dut.h"]
                )
                pipe_files = agent_pipe.generate_pipeline(
                    dut_files["Dut.h"], tb_files["Testbench.h"]
                )
                out_dir = LOG_ROOT / cpp.stem
                write_outputs(dut_files, tb_files, pipe_files, out_dir, cpp.stem)
            except Exception as e:
                print(f"[Error] {cpp.name}: {e}")
            print()
        return

    # =============================================================
    # B. Single‑file mode
    # =============================================================
    try:
        entry = extract_entry(src_path, args.function)
        dut_files = agent_dut.generate_dut(entry["code"])
        tb_files = agent_tb.generate_tb(
            dut_cpp=dut_files["Dut.cpp"], dut_h=dut_files["Dut.h"]
        )
        pipe_files = agent_pipe.generate_pipeline(
            dut_files["Dut.h"], tb_files["Testbench.h"]
        )
    except Exception as e:
        sys.exit(f"[Error] {e}")

    # Decide output location
    if args.out:
        custom_path = Path(args.out).expanduser()
        out_dir = custom_path.parent if custom_path.suffix else custom_path
    else:
        out_dir = LOG_ROOT / src_path.stem
    write_outputs(dut_files, tb_files, pipe_files, out_dir, src_path.stem)


if __name__ == "__main__":
    main()
