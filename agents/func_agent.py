import json
import os
import re
import sys
from pathlib import Path

from utils import VLLMGenerator

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
MODEL_NAME = os.getenv("FUNC_AGENT_MODEL", "Qwen/Qwen2.5-Coder-7B-Instruct")
_llm = VLLMGenerator(MODEL_NAME)

# ---------------------------------------------------------------------------
# Prompts & Examples
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT = (
    "You are Qwen, created by Alibaba Cloud. You are a precise code analyst. "
    "Extract every non-main function from provided C++ code, "
    "and output ONLY a JSON array of objects with keys `name`, `return_type`, and `code`."
)

_EXAMPLE_CODE = """
// example.cpp
#include <iostream>
// add two numbers
double add(double a, double b) {
    return a + b;
}
int main() { std::cout << add(2.0, 3.0); return 0; }
"""

_EXAMPLE_OUTPUT = json.dumps(
    [
        {
            "name": "add",
            "return_type": "double",
            "code": "double add(double a, double b) {\n    return a + b;\n}",
        }
    ],
    ensure_ascii=False,
)

_PROMPT_TEMPLATE = (
    "Now extract every non-main function from the following C++ code.\n"
    "Return a JSON array of objects with keys `name`, `return_type`, and `code`.\n"
    "Do not wrap in markdown or add explanations.\n"
    "Here is the code:\n```cpp\n{code}\n```"
)


# ---------------------------------------------------------------------------
# Public API
# ---------------------------------------------------------------------------
def extract_functions(src_path: str | Path, *, max_tokens: int = 2048) -> list[dict]:
    code = Path(src_path).read_text(encoding="utf-8", errors="ignore")
    user_prompt = _PROMPT_TEMPLATE.format(code=code)

    messages = [
        {"role": "system", "content": _SYSTEM_PROMPT},
        {"role": "user", "content": f"```cpp\n{_EXAMPLE_CODE}\n```"},
        {"role": "assistant", "content": _EXAMPLE_OUTPUT},
        {"role": "user", "content": user_prompt},
    ]

    formatted = _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )
    raw = _llm.generate(formatted, max_new_tokens=max_tokens)

    match = re.search(r"\[.*\]", raw, re.S)
    if not match:
        raise ValueError(
            "LLM output did not contain a JSON array.\n--- OUTPUT START ---\n"
            + raw
            + "\n--- OUTPUT END ---"
        )
    try:
        return json.loads(match.group(0))
    except json.JSONDecodeError as e:
        raise ValueError(f"Failed to parse JSON: {e}\nRaw output:\n{raw}")


# ---------------------------------------------------------------------------
# CLI Helper
# ---------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python func_agent.py <cpp_source_file>", file=sys.stderr)
        sys.exit(1)

    src_file = Path(sys.argv[1]).expanduser()
    if not src_file.exists():
        print(f"File not found: {src_file}", file=sys.stderr)
        sys.exit(1)

    try:
        functions = extract_functions(src_file)
        print(json.dumps(functions, ensure_ascii=False, indent=2))
    except Exception as exc:
        print(f"Error: {exc}", file=sys.stderr)
        sys.exit(2)
