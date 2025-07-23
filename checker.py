#!/usr/bin/env python3
"""
Batch SystemC checker with tqdm and per-test timeout.

States
------
compile_error   – CMake configure/build failed or binary missing
runtime_error   – binary exit code non-zero OR exceeded time limit
unit_test_pass  – binary exited 0 within the time limit
format_error    – project directory empty (no generated files)
"""
from __future__ import annotations

import argparse
import csv
import json
import os
import signal
import subprocess
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Use tqdm if available; fall back to a no-op iterator otherwise.
try:
    from tqdm import tqdm
except ModuleNotFoundError:  # pragma: no cover

    def tqdm(it, **kwargs):  # type: ignore
        return it


# ---------------------------------------------------------------------------


def run(cmd: list[str], cwd: Path, timeout: float | None = None) -> tuple[int, str]:
    """Execute a command and return (exit_code, output)."""
    try:
        proc = subprocess.run(
            cmd,
            cwd=cwd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,  # decode with locale encoding
            timeout=timeout,
        )
        return proc.returncode, proc.stdout
    except subprocess.TimeoutExpired as e:
        # Terminate the whole process group to avoid orphans.
        if getattr(e, "process", None):
            try:
                os.killpg(os.getpgid(e.process.pid), signal.SIGTERM)
            except Exception:
                pass
        output = e.stdout if e.stdout is not None else b""
        if isinstance(output, bytes):
            output = output.decode("utf-8", "replace")
        return -1, output


def compile_failed(cfg_rc: int, bld_rc: int) -> bool:
    return cfg_rc != 0 or bld_rc != 0


def classify_state(build_fail: bool, run_rc: int) -> str:
    if build_fail:
        return "compile_error"
    if run_rc != 0:
        return "runtime_error"
    return "unit_test_pass"


def safe_concat(a: str, b: str | bytes) -> str:
    """Concatenate two outputs, decoding bytes if necessary."""
    if isinstance(b, bytes):
        b = b.decode("utf-8", "replace")
    return a + b


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--root", default="./.log", help="Directory with one project per sub-directory"
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=3.0,
        help="Seconds allowed for each test-dut (default 3)",
    )
    parser.add_argument(
        "--json", default="data_output/results.json", help="Output JSON file"
    )
    parser.add_argument(
        "--csv", default="data_output/summary.csv", help="Output CSV file"
    )
    args = parser.parse_args()

    root = Path(args.root).expanduser().resolve()
    if not root.is_dir():
        sys.exit(f"Root directory not found: {root}")

    projects = sorted(p for p in root.iterdir() if p.is_dir())

    results: list[dict] = []
    counter = {
        "compile_error": 0,
        "runtime_error": 0,
        "unit_test_pass": 0,
        "format_error": 0,
    }

    for project in tqdm(projects, desc="Checking projects"):
        name = project.name
        state = "format_error"  # default, may be overridden
        combined_out = ""

        # --- 新增：空資料夾偵測 ---
        if not any(project.iterdir()):
            combined_out = (
                "Project directory is empty – likely extraction/formatting failure."
            )
            counter[state] += 1
            results.append(
                {
                    "name": name,
                    "state": state,
                    "error_message": combined_out,
                }
            )
            continue  # 不再嘗試 build / run

        build_dir = project / "build"
        try:
            # Configure
            cfg_rc, cfg_out = run(
                ["cmake", "-S", str(project), "-B", str(build_dir)], cwd=project
            )
            # Build
            bld_rc, bld_out = run(["cmake", "--build", str(build_dir)], cwd=project)

            build_fail = compile_failed(cfg_rc, bld_rc)
            combined_out = safe_concat(cfg_out, bld_out)

            run_rc = -1
            if not build_fail:
                exe = build_dir / "test-dut"
                if exe.is_file():
                    run_rc, run_out = run([str(exe)], cwd=project, timeout=args.timeout)
                    combined_out = safe_concat(combined_out, run_out)
                else:
                    build_fail = True  # treat as compile error

            state = classify_state(build_fail, run_rc)
        except Exception as err:  # catch any unexpected failure per project
            state = "runtime_error"
            combined_out = f"Unexpected error: {err}"

        counter[state] += 1
        results.append(
            {
                "name": name,
                "state": state,
                "error_message": (
                    "" if state == "unit_test_pass" else combined_out.strip()
                ),
            }
        )

    # Write JSON
    Path(args.json).write_text(json.dumps(results, indent=2), encoding="utf-8")

    # Write CSV summary
    total = sum(counter.values())
    pass_rate = counter["unit_test_pass"] / total if total else 0.0
    with open(args.csv, "w", newline="", encoding="utf-8") as fp:
        w = csv.writer(fp)
        w.writerow(["state", "count"])
        for s, c in counter.items():
            w.writerow([s, c])
        w.writerow([])
        w.writerow(["unit_test_pass_rate", f"{pass_rate:.2%}"])


if __name__ == "__main__":
    main()
