#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
main_pre.py — Extract → combine → reflexion pipeline for Stratus synthesis.

It can operate in two modes:

1. **Single‑file mode**: pass a path to a C/C++ translation unit (`.cpp`).
2. **Batch‑JSON mode**: pass a path to a JSON file formatted like `data_input_synth.json`.

For JSON mode each entry must contain::

    {
        "name": "ProblemName",
        "category": "Easy|Medium|Hard|...",
        "code": "<original C/C++ source>"
    }

Outputs are written to:

    <out_dir>/<category>/<name>/<name>.cpp
    <out_dir>/<category>/<name>/result.txt

Default `out_dir` is `.log` (same as the original script).
"""

from __future__ import annotations

import argparse
import json
import tempfile
from pathlib import Path
from typing import List, Dict

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
        help="Path to a C/C++ file **or** a data_input_synth‑style JSON file.",
    )
    ap.add_argument(
        "-o",
        "--out_dir",
        default=".log",
        help="Directory for generated artefacts.",
    )
    ap.add_argument(
        "--cpp_name",
        help="Override output cpp filename (single‑file mode only; without extension).",
    )
    ap.add_argument(
        "--category",
        help="Override category folder name (single‑file mode only). Default = 'misc'.",
        default="misc",
    )
    ap.add_argument("--max_iter", type=int, default=10)
    ap.add_argument("--temp_actor", type=float, default=0.7)
    ap.add_argument("--temp_env", type=float, default=0.7)
    ap.add_argument("--max_tokens_actor", type=int, default=4096)
    ap.add_argument("--max_tokens_env", type=int, default=2048)
    return ap


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _run_pipeline(
    code: str,
    cpp_stem: str,
    category: str,
    out_dir: Path,
    *,
    max_iter: int,
    temp_actor: float,
    temp_env: float,
    max_tokens_actor: int,
    max_tokens_env: int,
) -> None:
    """Extract functions from *code*, run reflexion loop, and write artefacts."""

    # --- Prepare temp file for agent_func (expects a file path) -------------
    with tempfile.NamedTemporaryFile(
        mode="w", suffix=".cpp", delete=False, encoding="utf-8"
    ) as tmp:
        tmp.write(code)
        tmp_path = Path(tmp.name)

    try:
        print(f"[Info] Extracting functions from: {cpp_stem} (category={category})")
        functions = agent_func.extract_functions(
            tmp_path, system_prompt=agent_func._SYSTEM_PROMPT
        )
        if not functions:
            raise RuntimeError("agent_func returned no functions.")

        combined_code = "\n\n".join(f["code"] for f in functions)

        print("[Info] Running reflexion loop …")
        final_output, record = agent_synth.run_reflexion(
            combined_code,
            max_iter=max_iter,
            temp_actor=temp_actor,
            temp_env=temp_env,
            max_tokens_actor=max_tokens_actor,
            max_tokens_env=max_tokens_env,
        )

        # --- Save -----------------------------------------------------------
        base_dir = out_dir / category / cpp_stem
        cpp_path = base_dir / f"{cpp_stem}.cpp"
        result_path = base_dir / "result.txt"

        record_text = "\n\n".join(record)
        result_content = (
            record_text + "\n\n===== FINAL REFLEXION OUTPUT =====\n" + final_output
        )

        _write(result_path, result_content)
        print(f"[Saved] result.txt → {result_path}")
        _write(cpp_path, final_output)
        print(f"[Saved] {cpp_stem}.cpp → {cpp_path}")
        print("[Done]")
    finally:
        # Clean up temp file
        try:
            tmp_path.unlink(missing_ok=True)
        except Exception:
            pass


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main() -> None:
    args = _build_cli().parse_args()
    src_path = Path(args.src).expanduser().resolve()
    out_dir = Path(args.out_dir).expanduser().resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    if src_path.suffix.lower() == ".json":
        print(f"[Mode] Batch‑JSON: {src_path}")
        with src_path.open("r", encoding="utf-8") as f:
            entries: List[Dict[str, str]] = json.load(f)

        for entry in entries:
            name = entry["name"]
            category = entry.get("category", "misc")
            code = entry["code"]
            _run_pipeline(
                code,
                cpp_stem=name,
                category=category,
                out_dir=out_dir,
                max_iter=args.max_iter,
                temp_actor=args.temp_actor,
                temp_env=args.temp_env,
                max_tokens_actor=args.max_tokens_actor,
                max_tokens_env=args.max_tokens_env,
            )
    else:
        # Single‑file mode ---------------------------------------------------
        print(f"[Mode] Single file: {src_path}")
        code = src_path.read_text(encoding="utf-8")
        cpp_stem = args.cpp_name if args.cpp_name else src_path.stem
        category = args.category
        _run_pipeline(
            code,
            cpp_stem=cpp_stem,
            category=category,
            out_dir=out_dir,
            max_iter=args.max_iter,
            temp_actor=args.temp_actor,
            temp_env=args.temp_env,
            max_tokens_actor=args.max_tokens_actor,
            max_tokens_env=args.max_tokens_env,
        )


if __name__ == "__main__":
    main()
