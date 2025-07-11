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
_SYSTEM_PROMPT = "You are Qwen, created by Alibaba Cloud. You are a senior SystemC/Stratus refactoring engineer and an exact C++ analyst."

_USER_PROMPT = (
    "Given the C++ code below, extract every non-main function from the following C++ code.\n"
    "Return a JSON array of objects with keys `name`, `return_type`, and `code`.\n"
    "Do not wrap in markdown or add explanations.\n"
    "Here is the code:\n```cpp\n{code}\n```"
)

_MULTI_STAGE_USER_PROMPT = (
    "Given the C++ code below, process the code in **three ordered stages**:\n"
    "1. **Function Builder** – Refactor any logic embedded in `main`, nested blocks, or complex expressions into well-named, stand-alone functions. "
    "Each new function must have clear parameters and (if needed) a single return value so overall behavior is unchanged but modularized.\n"
    "2. **Synthesis Rewriter** – Rewrite the fully refactored code so it is synthesizable by Cadence Stratus/SystemC. "
    "Remove or replace constructs Stratus cannot synthesize (e.g., recursion, dynamic memory, STL containers, `std::pow`, variable-bound loops, etc). "
    "3. **Function Extractor** – From the final synthesizable version, collect every function whose name is **not** `main`. "
    "Return them verbatim in a JSON array where each element is an object with keys: "
    '`"name"`, `"return_type"`, and `"code"` (the full function definition including signature and braces).\n\n'
    "**Output format (strict):**\n"
    "• Respond with a **single top-level JSON array** from stage 3.\n"
    "• Do **not** include the refactored C++ source, markdown fences, or any extra commentary—return **only** the JSON array.\n\n"
    "Here is the code:\n```cpp\n{code}\n```"
)

_STRUCTURE_FEW_SHOT_CODE_1 = """
// example.cpp
#include <iostream>
// add two numbers
double add(double a, double b) {
    return a + b;
}
int main() { std::cout << add(2.0, 3.0); return 0; }
"""

_STRUCTURE_FEW_SHOT_OUTPUT_1 = json.dumps(
    [
        {
            "name": "add",
            "return_type": "double",
            "code": "double add(double a, double b) {\n    return a + b;\n}",
        }
    ],
    ensure_ascii=False,
)

_STRUCTURE_FEW_SHOT_CODE_2 = """
// stats_utils.cpp
#include <iostream>
#include <cmath>

// accumulate array values
double calc_sum(const double* arr, size_t n) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i)
        s += arr[i];
    return s;
}

// mean = sum / n
double calc_mean(const double* arr, size_t n) {
    return calc_sum(arr, n) / static_cast<double>(n);
}

// variance = Σ(x - μ)^2 / n
double calc_variance(const double* arr, size_t n) {
    double mu = calc_mean(arr, n);
    double var = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double diff = arr[i] - mu;
        var += diff * diff;
    }
    return var / static_cast<double>(n);
}

int main() {
    double data[6] = {1.2, 3.4, 2.1, 4.8, 5.6, 3.3};
    std::cout << "mean     = " << calc_mean(data, 6)     << std::endl;
    std::cout << "variance = " << calc_variance(data, 6) << std::endl;
    return 0;
}
"""

_STRUCTURE_FEW_SHOT_OUTPUT_2 = json.dumps(
    [
        {
            "name": "calc_sum",
            "return_type": "double",
            "code": "double calc_sum(const double* arr, size_t n) {\n    double s = 0.0;\n    for (size_t i = 0; i < n; ++i)\n        s += arr[i];\n    return s;\n}",
        },
        {
            "name": "calc_mean",
            "return_type": "double",
            "code": "double calc_mean(const double* arr, size_t n) {\n    return calc_sum(arr, n) / static_cast<double>(n);\n}",
        },
        {
            "name": "calc_variance",
            "return_type": "double",
            "code": "double calc_variance(const double* arr, size_t n) {\n    double mu = calc_mean(arr, n);\n    double var = 0.0;\n    for (size_t i = 0; i < n; ++i) {\n        double diff = arr[i] - mu;\n        var += diff * diff;\n    }\n    return var / static_cast<double>(n);\n}",
        },
    ],
    ensure_ascii=False,
)


# ---------------------------------------------------------------------------
# Public API
# ---------------------------------------------------------------------------
def extract_functions(src_path: str | Path, *, max_tokens: int = 4096) -> list[dict]:
    code = Path(src_path).read_text(encoding="utf-8", errors="ignore")
    user_prompt = _MULTI_STAGE_USER_PROMPT.format(code=code)

    messages = [
        {"role": "system", "content": _SYSTEM_PROMPT},
        {
            "role": "user",
            "content": _MULTI_STAGE_USER_PROMPT.format(code=_STRUCTURE_FEW_SHOT_CODE_1),
        },
        {"role": "assistant", "content": _STRUCTURE_FEW_SHOT_OUTPUT_1},
        {
            "role": "user",
            "content": f"```{_MULTI_STAGE_USER_PROMPT}\n\ncpp\n{_STRUCTURE_FEW_SHOT_CODE_2}\n```",
        },
        {"role": "assistant", "content": _STRUCTURE_FEW_SHOT_OUTPUT_2},
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
