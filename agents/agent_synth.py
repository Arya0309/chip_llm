#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
agent_synth.py — Stratus synthesizability agent

CLI modes
---------
• report     – full analysis & (if needed) corrected code        (default)
• verify     – verdict only: [STATE] PASS|FAIL [/STATE]
• reflexion  – Actor/Environment loop; Environment feedback 餵回 Actor
"""

from __future__ import annotations
import argparse
import os
import re
import sys
from pathlib import Path
from typing import List, Dict

from utils import DEFAULT_MODEL, VLLMGenerator
import prompts as prompt  # parity with agent_dut (even if unused)

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
MODEL_NAME = os.getenv("LLM_MODEL", DEFAULT_MODEL)
_llm = VLLMGenerator(MODEL_NAME)

# ---------------------------------------------------------------------------
# Prompts
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT_REPORT = """
You are Qwen, created by Alibaba Cloud. You are an AI Stratus synthesizer.
First analyse whether the following C/C++ code can be synthesized and write
your reasoning inside:
[ANALYSIS]
...
[/ANALYSIS]
Then, *immediately after* the analysis block:
  • If the code **is** synthesizable, output the original code wrapped in a
    ```cpp``` fenced block.
  • If the code **is NOT** synthesizable, list the blocking issues inside the
    analysis, then provide a *corrected* synthesizable version, also wrapped in
    ```cpp```.
Do not output anything outside the [ANALYSIS]...[/ANALYSIS] block and the
single fenced code block that follows it.
"""

_SYSTEM_PROMPT_VERIFY = """
You are Qwen, created by Alibaba Cloud. You are an expert on Cadence Stratus HLS synthesizability.
First analyse whether the following C/C++ code can be synthesized and write your reasoning inside: 
(Note: Any statement or helper function that performs printing or I/O is NON-synthesizable.)
[FEEDBACK]
...
[/FEEDBACK]
Then, on a separate final line, output exactly one of:
[STATE] PASS [/STATE]   # if synthesizable
[STATE] FAIL [/STATE]   # if NOT synthesizable
"""

# ---------------------------------------------------------------------------
# Regex helpers
# ---------------------------------------------------------------------------
_CODE_RE = re.compile(r"```cpp\s*(.*?)```", re.S)
_STATE_RE = re.compile(r"\[STATE\]\s*(PASS|FAIL)\s*\[/STATE\]", re.I)
_FEEDBACK_RE = re.compile(r"\[FEEDBACK\](.*?)\[/FEEDBACK\]", re.S | re.I)

# ---------------------------------------------------------------------------
# Message builders
# ---------------------------------------------------------------------------


def _build_messages(
    source_code: str, *, mode: str, feedback: str | None = None
) -> List[Dict[str, str]]:
    """
    Build chat messages.
    • mode == 'report'  : use _SYSTEM_PROMPT_REPORT
    • mode == 'verify'  : use _SYSTEM_PROMPT_VERIFY
    • feedback != None  : prepend previous analysis to user message
    """
    sys_prompt = _SYSTEM_PROMPT_REPORT if mode == "report" else _SYSTEM_PROMPT_VERIFY

    if feedback:
        user_content = (
            f"/* Previous verifier feedback */\n{feedback.strip()}\n\n"
            "/* Code to refine */\n"
            f"```cpp\n{source_code.strip()}\n```"
        )
    else:
        if mode == "verify":
            user_content = (
                "Please verify the synthesizability of the following C/C++ code and write your reasoning inside the [FEEDBACK]...[/FEEDBACK] block:\n"
                "/* Code to verify synthesizability */\n"
                f"```cpp\n{source_code.strip()}\n```"
            )
        else:
            user_content = f"```cpp\n{source_code.strip()}\n```"

    return [
        {"role": "system", "content": sys_prompt},
        {"role": "user", "content": user_content},
    ]


def _chat_to_prompt(messages: List[Dict[str, str]]) -> str:
    """Convert chat messages to a single prompt string using vLLM template."""
    return _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )


# ---------------------------------------------------------------------------
# Generation APIs
# ---------------------------------------------------------------------------


def _llm_generate(prompt_text: str, *, temperature: float, max_new_tokens: int) -> str:
    return _llm.generate(
        prompt_text,
        temperature=temperature,
        max_new_tokens=max_new_tokens,
        use_tqdm=False,
    ).strip()


def generate_synth(
    source_code: str,
    *,
    temperature: float = 0.3,
    max_new_tokens: int = 2048,
    feedback: str | None = None,
) -> str:
    """Full report; optional feedback for Reflexion."""
    prompt = _chat_to_prompt(
        _build_messages(source_code, mode="report", feedback=feedback)
    )
    return _llm_generate(prompt, temperature=temperature, max_new_tokens=max_new_tokens)


def generate_synth_verifier(
    source_code: str, *, temperature: float = 0.3, max_new_tokens: int = 2048
) -> str:
    prompt = _chat_to_prompt(_build_messages(source_code, mode="verify"))
    return _llm_generate(prompt, temperature=temperature, max_new_tokens=max_new_tokens)


def generate_synth_batch(
    sources: List[str], *, temperature: float = 0.3, max_new_tokens: int = 2048
) -> List[str]:
    prompts = [_chat_to_prompt(_build_messages(src, mode="report")) for src in sources]
    outs = _llm.generate_batch(
        prompts, temperature=temperature, max_new_tokens=max_new_tokens
    )
    return [s.strip() for s in outs]


# ---------------------------------------------------------------------------
# Reflexion loop
# ---------------------------------------------------------------------------


def _extract(regex: re.Pattern, text: str) -> str | None:
    m = regex.search(text)
    return m.group(1).strip() if m else None


def run_reflexion(
    original_code: str,
    *,
    max_iter: int = 10,
    temp_actor: float = 0.3,
    temp_env: float = 0.0,
    max_tokens_actor: int = 2048,
    max_tokens_env: int = 1024,
):
    """Actor–Environment loop with feedback."""
    current_code = original_code
    current_feedback: str | None = None  # no feedback in first round
    record: List[str] = []  # to store all reports

    for step in range(1, max_iter + 1):
        message = f"\n===== Reflexion Step {step} ====="
        print(message)
        record.append(message)

        # --- Actor ---
        message = "[Actor] Generating report...\n"
        print(message)
        record.append(message)
        report = generate_synth(
            current_code,
            temperature=temp_actor,
            max_new_tokens=max_tokens_actor,
            feedback=current_feedback,
        )
        print(report)
        record.append(report)

        code_block = _extract(_CODE_RE, report)
        if not code_block:
            message = "[Reflexion] No ```cpp``` block found in actor output. Abort."
            print(message)
            record.append(message)
            return report, record

        # --- Environment ---
        message = "\n[Environment] Verifying synthesizability...\n"
        print(message)
        record.append(message)

        verdict = generate_synth_verifier(
            code_block,
            temperature=temp_env,
            max_new_tokens=max_tokens_env,
        )
        print(verdict)
        record.append(verdict)

        state = _extract(_STATE_RE, verdict)
        feedback = _extract(_FEEDBACK_RE, verdict)
        if not feedback:
            message = "[Reflexion] No [FEEDBACK] section found in verifier output."
            print(message)
            record.append(message)
        if state == "PASS":
            message = "\n[Reflexion] PASS! Returning final code."
            print(message)
            record.append(message)
            return code_block, record

        message = "[Reflexion] FAIL. Feeding feedback back to Actor."
        print(message)
        record.append(message)
        # 下一輪
        current_code = code_block
        current_feedback = f"[FEEDBACK]\n{feedback}\n[/FEEDBACK]"

    message = "[Reflexion] Max iterations reached. Returning last report."
    record.append(message)
    print(message)
    return report, record


# ---------------------------------------------------------------------------
# CLI helpers
# ---------------------------------------------------------------------------


def _read_source(path: Path) -> str:
    if path.suffix.lower() in {".c", ".cc", ".cpp", ".h", ".hpp"}:
        return path.read_text(encoding="utf-8")
    raise ValueError(f"Unsupported file type: {path}")


def _reinit_llm_if_needed(model_name: str | None) -> None:
    global MODEL_NAME, _llm
    if model_name and model_name != MODEL_NAME:
        MODEL_NAME = model_name
        os.environ["LLM_MODEL"] = MODEL_NAME
        _llm = VLLMGenerator(MODEL_NAME)


# ---------------------------------------------------------------------------
# Output helpers
# ---------------------------------------------------------------------------


def _prepare_out_dir(out_root: Path, source_label: str) -> Path:
    """Create <out_root>/<source_label>/ directory and return it."""
    out_dir = out_root / source_label
    out_dir.mkdir(parents=True, exist_ok=True)
    return out_dir


def _write_outputs(
    out_dir: Path, result: str, record: List[str], source_label: str
) -> None:
    (out_dir / f"{source_label}.cpp").write_text(result, encoding="utf-8")
    (out_dir / "record.txt").write_text("\n\n".join(record), encoding="utf-8")


# ---------------------------------------------------------------------------
# CLI entry
# ---------------------------------------------------------------------------


def main(argv: List[str] | None = None) -> None:
    parser = argparse.ArgumentParser(
        description="Stratus synthesizability agent (report / verify / reflexion)."
    )
    parser.add_argument(
        "--mode",
        choices=("report", "verify", "reflexion"),
        default="report",
        help="report (analysis & fix), verify (PASS/FAIL), reflexion (loop).",
    )
    parser.add_argument(
        "source", help="Path to a C/C++ file, or '-' to read from stdin."
    )
    parser.add_argument(
        "-m",
        "--model",
        default="",
        help="Override model (env LLM_MODEL → DEFAULT_MODEL).",
    )
    parser.add_argument("--temperature", type=float, default=0.3)
    parser.add_argument("--max-new-tokens", type=int, default=4096)
    parser.add_argument(
        "--max-iter", type=int, default=5, help="Max reflexion iterations."
    )
    parser.add_argument(
        "-o",
        "--out-dir",
        default=".log",
        help="Output root directory (default: .log/<input_cpp_name>).",
    )

    args = parser.parse_args(argv)
    _reinit_llm_if_needed(args.model or os.getenv("LLM_MODEL"))

    # Determine label for output subdirectory
    if args.source == "-":
        source_label = "stdin"
        source_code = sys.stdin.read()
    else:
        source_path = Path(args.source)
        source_label = source_path.stem
        source_code = _read_source(source_path)

    out_root = Path(args.out_dir)
    out_dir = _prepare_out_dir(out_root, source_label)

    # ---- dispatch ----
    if args.mode == "verify":
        result = generate_synth_verifier(
            source_code,
            temperature=0.0,
            max_new_tokens=min(args.max_new_tokens, 2048),
        )
        record = [result]
    elif args.mode == "reflexion":
        result, record = run_reflexion(
            source_code,
            max_iter=args.max_iter,
            temp_actor=args.temperature,
            temp_env=0.0,
            max_tokens_actor=args.max_new_tokens,
            max_tokens_env=min(args.max_new_tokens, 2048),
        )
    else:  # report
        result = generate_synth(
            source_code,
            temperature=args.temperature,
            max_new_tokens=args.max_new_tokens,
        )
        record = [result]

    # Persist outputs
    _write_outputs(out_dir, result, record, source_label)

    # Console echo
    print("\n===== Final Output =====")
    print(result)
    print(f"\n[Saved] Results are stored under: {out_dir}\n")


if __name__ == "__main__":
    main()
