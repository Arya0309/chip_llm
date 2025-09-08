from __future__ import annotations

import argparse
import json
import os
import re
import sys
from pathlib import Path
from typing import List, Dict, Tuple, Any

from transformers import AutoTokenizer, AutoModelForCausalLM
from utils import DEFAULT_MODEL, VLLMGenerator, HFGenerator
import prompts as prompt  # parity with agent_dut (even if unused)

from openai import OpenAI, BadRequestError


_oa_client = OpenAI(  # 如果沒設就會自動讀 OPENAI_API_KEY 環境變數
    api_key=os.getenv("OPENAI_API_KEY", None)
)
# ------------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------------
MODEL_NAME = os.getenv("LLM_MODEL", DEFAULT_MODEL)
_llm: VLLMGenerator | HFGenerator | None = None


def _init_llm(model_name: str) -> None:
    """Initialise the generator: vLLM if可用，否則自動轉用 HF."""
    global MODEL_NAME, _llm
    MODEL_NAME = model_name or DEFAULT_MODEL
    os.environ["LLM_MODEL"] = MODEL_NAME

    try:
        _llm = VLLMGenerator(MODEL_NAME)  # 先嘗試 vLLM
    except Exception as e:
        print(f"[Fallback] vLLM unavailable ({e}); switching to HuggingFace backend.")
        _llm = HFGenerator(MODEL_NAME)


# ------------------------------------------------------------------------
# Prompts
# ------------------------------------------------------------------------
_SYSTEM_PROMPT_REPORT = """
You are an AI Stratus synthesizer.
First analyse whether the following C/C++ code can be synthesized and write your reasoning inside:
[ANALYSIS]
...
[/ANALYSIS]
Then, *immediately after* the analysis block:
  • If the code **is** synthesizable, output the original C++ code wrapped in a ```cpp``` fenced block.
  • If the code **is NOT** synthesizable, list the blocking issues inside the analysis, then provide a *corrected* synthesizable version C++ code, also wrapped in ```cpp```.
Do not output anything outside the [ANALYSIS]...[/ANALYSIS] block and the single fenced code block that follows it.
"""

# System prompt version 1
# _SYSTEM_PROMPT_VERIFY = r"""
# Act as a **conservative Cadence Stratus HLS synthesizability checker**.

# Carefully inspect the following C/C++ translation unit. Follow the rules below:

# 1. Read the code **twice**. Do not assume the existence of any undefined symbols.
# 2. **Only** consider the following categories as synthesizability violations.
#    • any I/O (printf/scanf, cin/cout, fprintf, file access)
#    • recursion (direct or indirect)
#    • dynamic memory (new, delete, malloc, free, VLAs)
#    • Unbounded or data-dependent loops without a statically provable finite upper bound. Provide a compile-time constant bound (via constants/templates/fixed-size arrays) or refactor the loop.
#    • calls to library/math/STL functions Stratus cannot inline (pow, exp, log, std::swap, sort, etc.)
#    • STL containers or algorithms (vector, list, map, string, std::function, …)
#    • general-purpose pointers used as memory, or pointer arithmetic beyond fixed arrays
#    • undefined-behaviour patterns (e.g. negative shift, out-of-bounds access)
# 3. **If any item in step 2 is present, or if you are NOT 100 % certain the design will synthesize, conclude FAIL.**
# 4. Only when **no violations** are found and you are completely confident, conclude PASS.

# Output exactly:

# [FEEDBACK]
# <bullet list of findings; if none, write “No violations found.”>
# [/FEEDBACK]
# Then, on a separate final line, output **one and only one** of:
# [STATE] PASS [/STATE]      # only when absolutely sure
# [STATE] FAIL [/STATE]

# Do **not** output anything else outside the required tags.
# """

# System prompt version 2
_SYSTEM_PROMPT_VERIFY = r"""
Act as a **conservative Cadence Stratus HLS synthesizability checker**.

Carefully inspect the following C/C++ translation unit. Follow the rules below:

1. Read the code **twice**. Do not assume the existence of any undefined symbols.
2. **Only** consider the following categories as synthesizability violations. 
   • any I/O (printf/scanf, cin/cout, fprintf, file access, iostream, bits/stdc++.h)  
   • recursion (direct or indirect)  
   • dynamic memory (new, delete, malloc, free, VLAs)  
   • Unbounded or data-dependent loops without a statically provable finite upper bound.
   • STL containers or algorithms (vector, list, map, string, std::function, …)  
   • general-purpose pointers used as memory, or pointer arithmetic beyond fixed arrays  
   • undefined-behaviour patterns (e.g. negative shift, out-of-bounds access)  
3. **If any item in step 2 is present, or if you are NOT 100 % certain the design will synthesize, conclude FAIL.**
4. Only when **no violations** are found and you are completely confident, conclude PASS.

Output exactly:

[FEEDBACK]
<bullet list of findings; if none, write “No violations found.”>
[/FEEDBACK]
Then, on a separate final line, output **one and only one** of:
[STATE] PASS [/STATE]      # only when absolutely sure
[STATE] FAIL [/STATE]

Do **not** output anything else outside the required tags.
"""

# ------------------------------------------------------------------------
# Regex helpers
# ------------------------------------------------------------------------
_CODE_RE = re.compile(r"```cpp\s*(.*?)```", re.S)
_STATE_RE = re.compile(r"\[STATE\]\s*(PASS|FAIL)\s*\[/STATE\]", re.I)
_FEEDBACK_RE = re.compile(r"\[FEEDBACK\](.*?)\[/FEEDBACK\]", re.S | re.I)


# ------------------------------------------------------------------------
# Message builder
# ------------------------------------------------------------------------
def _build_messages(
    source_code: str,
    *,
    mode: str,
    feedback: str | None = None,
    model: str | None = None,
) -> List[Dict[str, str]]:
    """Return OpenAI-style chat messages."""
    sys_prompt = _SYSTEM_PROMPT_REPORT if mode == "report" else _SYSTEM_PROMPT_VERIFY
    model = model or MODEL_NAME  # fallback to most-recent global

    if "qwen" in model.lower():
        sys_prompt = prompt._QWEN_SYSTEM_PROMPT_HEAD + sys_prompt

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
    """Convert chat messages to vLLM prompt string."""
    return _llm.apply_chat_template(  # type: ignore
        messages, tokenize=False, add_generation_prompt=True
    )


# ------------------------------------------------------------------------
# Local generation helpers
# ------------------------------------------------------------------------
def _llm_generate(prompt_text: str, *, temperature: float, max_new_tokens: int) -> str:
    return _llm.generate(  # type: ignore
        prompt_text,
        temperature=temperature,
        max_new_tokens=max_new_tokens,
        use_tqdm=False,
    ).strip()


# ------------------------------------------------------------------------
# Writer / Verifier (single & batch)
# ------------------------------------------------------------------------
def generate_writer(
    source_code: str,
    *,
    temperature: float = 0.3,
    max_new_tokens: int = 2048,
    feedback: str | None = None,
) -> str:
    prompt = _chat_to_prompt(
        _build_messages(source_code, mode="report", feedback=feedback, model=MODEL_NAME)
    )
    return _llm_generate(prompt, temperature=temperature, max_new_tokens=max_new_tokens)


def generate_verifier(
    source_code: str, *, temperature: float = 0.3, max_new_tokens: int = 2048
) -> str:
    prompt = _chat_to_prompt(
        _build_messages(source_code, mode="verify", model=MODEL_NAME)
    )
    return _llm_generate(prompt, temperature=temperature, max_new_tokens=max_new_tokens)


# --------------------------------------------------------------------
# OpenAI verifier (optional, new-style client API)
# --------------------------------------------------------------------
def verify_with_openai(
    code_cpp: str,
    *,
    model_name: str,
    temperature: float = 0.0,
    max_tokens: int = 1024,
) -> str:
    """Run the strict VERIFY prompt on OpenAI model."""
    if not model_name:
        raise ValueError("verify_with_openai called with empty model_name")

    messages = _build_messages(code_cpp, mode="verify", model=model_name)
    resp = _oa_client.chat.completions.create(
        model=model_name,
        messages=messages,
        temperature=temperature,
        max_tokens=max_tokens,
    )
    return resp.choices[0].message.content.strip()


# ------------------------------------------------------------------------
# Reflexion loop (single)
# ------------------------------------------------------------------------
def _extract(regex: re.Pattern, text: str) -> str | None:
    m = regex.search(text)
    return m.group(1).strip() if m else None


def _extract_last_nonempty(regex: re.Pattern, text: str) -> str | None:
    last = None
    for m in regex.finditer(text):
        s = m.group(1).strip()
        if s:
            last = s
    return last


def run_reflexion(
    original_code: str,
    *,
    max_iter: int = 10,
    temp_actor: float = 0.3,
    temp_env: float = 0.3,
    max_tokens_actor: int = 4096,
    max_tokens_env: int = 4096,
) -> Tuple[str, List[str]]:

    current_code = original_code.split("int main()")[0].strip()
    record: List[str] = []
    current_feedback: str | None = None

    record.append(f"[Original Code]\n{current_code}\n")

    step = 0
    while step <= max_iter:
        record.append(f"===== Reflexion Step {step} =====")

        # ---- Environment：驗證現有程式碼 ----
        record.append("[Environment] Verifying synthesizability…")
        verdict = generate_verifier(
            current_code, temperature=temp_env, max_new_tokens=max_tokens_env
        )
        try:
            verdict = verdict.split(
                "<|end|><|start|>assistant<|channel|>final<|message|>"
            )[1]
            verdict = verdict.split("<|return|>")[0]
        except IndexError:
            pass

        record.append(verdict)

        state = _extract(_STATE_RE, verdict)
        feedback = _extract(_FEEDBACK_RE, verdict)

        if state == "PASS" and step != 0:  # step=0 PASS 有可能是幻覺
            record.append("[Reflexion] PASS! Returning final code.")
            return current_code, record

        if not feedback:
            record.append("[Reflexion] FAIL but no feedback; retrying this step.")
            continue  # 不前進 step，重跑同一輪

        # ---- Actor：根據回饋產生新程式碼 ----
        current_feedback = f"[FEEDBACK]\n{feedback}\n[/FEEDBACK]"
        record.append("[Reflexion] FAIL. Feeding feedback to Actor.")
        record.append("[Actor] Generating report…")

        report = generate_writer(
            current_code,
            temperature=temp_actor,
            max_new_tokens=max_tokens_actor,
            feedback=current_feedback,
        )
        try:
            report = report.split(
                "<|end|><|start|>assistant<|channel|>final<|message|>"
            )[1]
            report = report.split("<|return|>")[0]
        except IndexError:
            pass
        record.append(report)

        code_block = _extract_last_nonempty(_CODE_RE, report)
        if not code_block:
            record.append(
                "[Reflexion] No ```cpp``` block found. Notifying Actor to retry."
            )

            # --- build base messages the SAME way writer did ---
            msgs = _build_messages(
                current_code,
                mode="report",
                feedback=current_feedback,
                model=MODEL_NAME,
            )

            # Keep only the tail of the previous reply to avoid blowing the context window
            def _tail(s: str, max_chars: int = 4000) -> str:
                return s[-max_chars:] if len(s) > max_chars else s

            # Add previous assistant output (shortened) as an assistant turn
            msgs.append({"role": "assistant", "content": _tail(report)})

            # English retry note: code-only, single fence, no analysis/comments
            retry_note = (
                "The previous reply either exceeded the token budget or failed to follow the instructions.\n"
                "Now output ONLY the final solution as a SINGLE code block in C++.\n"
                "Do not include any analysis, explanations, or comments.\n"
                "Format strictly:\n"
                "```cpp\n<code>\n```"
            )
            msgs.append({"role": "user", "content": retry_note})

            retry_prompt = _chat_to_prompt(msgs)
            report_retry = _llm_generate(
                retry_prompt,
                temperature=temp_actor,
                max_new_tokens=int(max_tokens_actor * 1.2),
            )
            record.append(report_retry)

            code_block = _extract_last_nonempty(_CODE_RE, report_retry)
            if not code_block:
                record.append(
                    "[Reflexion] Retry still produced no ```cpp``` block. Abort."
                )
                return current_code, record

        # 下一回合將會驗證這份 Actor 產生的程式碼
        current_code = code_block
        step += 1  # 只有成功產生 feedback + code 時才推進

    record.append("[Reflexion] Max iterations reached. Returning last code.")
    return current_code, record


# ------------------------------------------------------------------------
# I/O helpers
# ------------------------------------------------------------------------
def _prepare_out_dir(out_root: Path, label: str) -> Path:
    out_dir = out_root / label
    out_dir.mkdir(parents=True, exist_ok=True)
    return out_dir


def _write_outputs(
    out_dir: Path, result: str, record: List[str], label: str, mode: str
) -> None:
    """Write .cpp (if not verify-only) and record.txt."""
    if mode != "verify":
        (out_dir / f"{label}.cpp").write_text(result, encoding="utf-8")
    (out_dir / "record.txt").write_text("\n\n".join(record), encoding="utf-8")


# ------------------------------------------------------------------------
# Core executor
# ------------------------------------------------------------------------
def _execute_single(
    *,
    source_code: str,
    label: str,
    args: argparse.Namespace,
    out_root: Path,
) -> Dict[str, Any]:
    """Run one input; return summary dict for consolidated JSON."""
    out_dir = _prepare_out_dir(out_root, label)

    summary: Dict[str, Any] = {
        "name": label,
        "steps": 0,
        "local_pass": False,
        "gpt_pass": None,
        "code": "",
    }

    # ---------------- mode: verify ----------------
    if args.mode == "verify":
        result = generate_verifier(
            source_code,
            temperature=0.0,
            max_new_tokens=min(args.max_new_tokens, 2048),
        )
        record = [result]
        summary["local_pass"] = _extract(_STATE_RE, result) == "PASS"

    # ---------------- mode: reflexion -------------
    elif args.mode == "reflexion":
        result, record = run_reflexion(
            source_code,
            max_iter=args.max_iter,
            temp_actor=args.temperature,
            temp_env=args.temperature,
            max_tokens_actor=args.max_new_tokens,
            max_tokens_env=min(args.max_new_tokens, 2048),
        )
        # Count steps & local pass
        summary["steps"] = sum(
            1 for line in record if line.startswith("===== Reflexion Step")
        )
        summary["local_pass"] = any(
            "PASS! Returning final code." in line for line in record
        )

        # ---------- optional GPT verifier ----------
        if args.oa_model:
            oa_verdict = verify_with_openai(result, model_name=args.oa_model)
            record.append("\n[OpenAI-Verifier] result:")
            record.append(oa_verdict)

            gpt_pass = _extract(_STATE_RE, oa_verdict) == "PASS"
            summary["gpt_pass"] = gpt_pass

            if not gpt_pass:
                record.append(
                    "[Warning] OpenAI verifier reports FAIL. Marking as NOT-synthesizable."
                )

    # ---------------- mode: report ----------------
    else:  # report
        result = generate_writer(
            source_code,
            temperature=args.temperature,
            max_new_tokens=args.max_new_tokens,
        )
        record = [result]

        # ---------- optional GPT verifier ----------
        if args.oa_model:
            result_for_verify = (
                _extract(_CODE_RE, result) or result
            )  # fallback to full report
            oa_verdict = verify_with_openai(result_for_verify, model_name=args.oa_model)
            record.append("\n[OpenAI-Verifier] result:")
            record.append(oa_verdict)

            gpt_pass = _extract(_STATE_RE, oa_verdict) == "PASS"
            summary["gpt_pass"] = gpt_pass

            if not gpt_pass:
                record.append(
                    "[Warning] OpenAI verifier reports FAIL. "
                    "Marking as NOT-synthesizable."
                )

    # ---------- write artefacts ----------
    _write_outputs(out_dir, result, record, label, args.mode)
    print(f"[Saved] Outputs for '{label}' → {out_dir}")

    summary["code"] = result
    return summary


# ------------------------------------------------------------------------
# CLI entry
# ------------------------------------------------------------------------
def _read_source(path: Path) -> str:
    if path.suffix.lower() in {".c", ".cc", ".cpp", ".h", ".hpp"}:
        return path.read_text(encoding="utf-8")
    raise ValueError(f"Unsupported file type: {path}")


def main(argv: List[str] | None = None) -> None:
    parser = argparse.ArgumentParser(
        description="Stratus synthesizability agent (report / verify / reflexion / reflexion-batch)."
    )
    parser.add_argument(
        "source", help="Path to a C/C++ file, a JSON batch file, or '-' for stdin."
    )
    parser.add_argument(
        "--mode",
        choices=("report", "verify", "reflexion"),
        default="reflexion",
    )
    parser.add_argument(
        "-m",
        "--model",
        default="openai/gpt-oss-20b",
        help="Override model (env LLM_MODEL → DEFAULT_MODEL).",
    )
    parser.add_argument("--temperature", type=float, default=0.3)
    parser.add_argument("--max-new-tokens", type=int, default=8192)
    parser.add_argument(
        "--max-iter", type=int, default=10, help="Max reflexion iterations."
    )
    parser.add_argument(
        "-o", "--out-dir", default=".log", help="Output root directory."
    )
    parser.add_argument(
        "--oa-model",
        default="",  # empty string → GPT verifier disabled
        help="OpenAI model for final verification (e.g. o1, gpt-4o). Leave blank to disable.",
    )

    args = parser.parse_args(argv)

    _init_llm(args.model or os.getenv("LLM_MODEL", DEFAULT_MODEL))

    out_root = Path(args.out_dir).expanduser().resolve()
    out_root.mkdir(parents=True, exist_ok=True)

    # ---------------- stdin as single ----------------
    if args.source == "-":
        source_text = sys.stdin.read()
        _execute_single(
            source_code=source_text,
            label="stdin",
            args=args,
            out_root=out_root,
        )
        return

    src_path = Path(args.source).expanduser().resolve()

    # ---------------- batch JSON ---------------------
    if src_path.suffix.lower() == ".json":
        print(f"[Mode] Batch-JSON: {src_path}")
        entries: List[Dict[str, str]] = json.loads(src_path.read_text(encoding="utf-8"))

        for e in entries:
            io = (e.get("IO_requirement") or "").strip()
            if io:
                # Escape any accidental "*/" to avoid breaking the comment
                safe_io = io.replace("*/", "*\\/")
                hint_block = "/* === IO REQUIREMENT (HINT) ===\n" + safe_io + "\n*/\n"
                code_text = e.get("code", "")
                if not code_text.lstrip().startswith(
                    "/* === IO REQUIREMENT (HINT) ==="
                ):
                    e["code"] = hint_block + code_text

        summaries: List[Dict[str, Any]] = []

        # 保留原本逐一處理（report / verify / reflexion）
        for entry in entries:
            name = entry["name"]
            code = entry["code"]

            summaries.append(
                _execute_single(
                    source_code=code,
                    label=name,
                    args=args,
                    out_root=out_root,
                )
            )

        # Consolidated summary
        summary_path = out_root / "summary.json"
        summary_path.write_text(
            json.dumps(summaries, ensure_ascii=False, indent=2), encoding="utf-8"
        )
        print(f"[Summary] Written to {summary_path}")

    # ---------------- single file --------------------
    else:
        print(f"[Mode] Single-file: {src_path}")
        source_code = _read_source(src_path)
        label = src_path.stem
        _execute_single(
            source_code=source_code,
            label=label,
            args=args,
            out_root=out_root,
        )

    print(f"\n[Done] All outputs are under: {out_root}\n")


if __name__ == "__main__":
    main()
