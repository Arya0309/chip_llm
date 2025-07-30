#!/usr/bin/env python3
"""
Convert C++ ⇒ SystemC via LLM agents (JSON‑only version)
========================================================

Usage
-----
python main.py path/to/batch.json              # outputs to <project_root>/.log
python main.py path/to/batch.json -o /tmp/out  # custom output root

The JSON must be a list of objects:
  • "name"        – identifier (required)
  • "code"        – raw C/C++ source (required)
  • "new_name"    – override output folder name (optional)
  • "requirement" – extra hint forwarded to LLM agents (optional)

For every entry six files are generated:
  Dut.[cpp|h], Testbench.[cpp|h], SystemPipeline.[cpp|h]
  plus helper files (CMakeLists.txt, main.cpp, testcases.txt, golden.txt)
  under:
     <out_dir>/<entry_name or new_name>/
"""

from __future__ import annotations

import argparse
import contextlib
import json
import os
import sys
from pathlib import Path
from tempfile import NamedTemporaryFile
from typing import Union

# --------------------------------------------------------------------------- #
# Early CLI to allow --model before heavy imports
early = argparse.ArgumentParser(add_help=False)
early.add_argument("--model", dest="llm_model")
early_args, _ = early.parse_known_args()
if early_args.llm_model:
    os.environ["LLM_MODEL"] = early_args.llm_model
# --------------------------------------------------------------------------- #

import utils
import agent_dut
import agent_func
import agent_tb
import agent_pipe

# ---------- Constants ----------
PROJECT_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_LOG_ROOT = PROJECT_ROOT / ".log"
# --------------------------------


# ----------------- CLI -----------------
def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(
        parents=[early],
        description="Convert C++ ⇒ SystemC via LLM agents (JSON‑only)",
    )
    ap.add_argument(
        "json_path",
        metavar="JSON",
        help="Path to batch.json (list of translation units)",
    )
    ap.add_argument(
        "-o",
        "--out_dir",
        default=str(DEFAULT_LOG_ROOT),
        help=f"Root directory to store outputs (default: {DEFAULT_LOG_ROOT})",
    )
    return ap.parse_args()


# --------------- Helpers ----------------
def load_json_entries(p: Path) -> list[dict]:
    data = json.loads(p.read_text(encoding="utf-8"))
    if not isinstance(data, list):
        raise ValueError("JSON must be a list of objects")
    return data


def extract_entry(cpp_src: Union[str, Path], func_name: str | None):
    """Return {name, code} where *code* contains one or more non‑main functions."""
    # ------ Path branch (rarely used, kept for completeness) ------
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
                tmp_path.unlink(missing_ok=True)

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
    """Write Dut.*, Testbench.*, SystemPipeline.* and helper files into *out_dir*."""
    out_dir.mkdir(parents=True, exist_ok=True)

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
    # Helpers
    (out_dir / "CMakeLists.txt").write_text(utils.get_cmake_list(), encoding="utf-8")
    (out_dir / "main.cpp").write_text(utils.get_main_cpp(), encoding="utf-8")
    (out_dir / "testcases.txt").write_text(
        utils.get_testcases(task_name or "default"), encoding="utf-8"
    )
    (out_dir / "golden.txt").write_text(
        utils.get_golden(task_name or "default"), encoding="utf-8"
    )

    print(f"[OK] All files written to {out_dir}")


# ----------------- Main -----------------
def main() -> None:
    args = parse_args()
    json_path = Path(args.json_path).expanduser()

    if not json_path.exists():
        sys.exit(f"[Error] File not found: {json_path}")
    if json_path.suffix.lower() != ".json":
        sys.exit("[Error] Only JSON batch mode is supported in this version.")

    out_root = Path(args.out_dir).expanduser()
    out_root.mkdir(parents=True, exist_ok=True)

    entries = load_json_entries(json_path)
    for item in entries:
        name = item.get("new_name") or item["name"]
        print(f"--- Processing {name} ---")
        requirement = item.get("requirement", "")

        out_dir = out_root / name

        def retry(fn, max_try=5, *, tag):
            for i in range(1, max_try + 1):
                try:
                    return fn()
                except Exception as e:
                    print(f"[{tag}] attempt {i}/{max_try} failed: {e}")

        entry = retry(
            lambda: extract_entry(item["code"], item.get("function")),
            tag="extract_entry",
        )

        dut_files = retry(
            lambda: agent_dut.generate_dut(entry["code"], requirement), tag="DUT"
        )

        tb_files = retry(
            lambda: agent_tb.generate_tb(
                dut_cpp=dut_files["Dut.cpp"],
                dut_h=dut_files["Dut.h"],
                requirement=requirement,
            ),
            tag="Testbench",
        )

        pipe_files = retry(
            lambda: agent_pipe.generate_pipeline(
                dut_files["Dut.h"], tb_files["Testbench.h"]
            ),
            tag="Pipeline",
        )

        try:
            write_outputs(dut_files, tb_files, pipe_files, out_dir, name)
        except Exception as e:
            print(f"[Error] {name}: {e}")


if __name__ == "__main__":
    main()
