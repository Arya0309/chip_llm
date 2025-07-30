import json
import os
import re
import sys
from pathlib import Path
from typing import List, Dict

from utils import DEFAULT_MODEL, VLLMGenerator

import prompts as prompt

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
MODEL_NAME = os.getenv("LLM_MODEL", DEFAULT_MODEL)
_llm = VLLMGenerator(MODEL_NAME)

# ---------------------------------------------------------------------------
# Prompts & Examples
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT = (
    "You are a senior SystemC/Stratus refactoring engineer and an exact C++ analyst."
)


_SYSTEM_PROMPT_V2 = """
Role
----
• Senior C++ refactoring engineer & SystemC / Cadence, Stratus synthesis specialist.  
• Your sole task is to analyse ONE translation unit and deliver synthesizable functions.

Workflow (three mandatory stages)
1. **Function Builder** – Move any logic embedded in `main`, complex expressions, or
   nested scopes into well‑named, top‑level functions (single return value each).
2. **Synthesis Rewriter** – Modify the entire file so it can pass Cadence Stratus HLS.
   ✗ No recursion  
   ✗ No dynamic memory / STL containers  
   ✗ No `std::pow` or other runtime‑bounded loops  
   ✗ **No console or file I/O** (e.g. `std::cout`, `std::cerr`, `printf`, `<fstream>`).  
     If the original code prints results, replace that print with either  
       a) returning the value, or  
       b) writing via an explicit reference / pointer output parameter — so the
          function becomes pure computation.
3. **Function Extractor** – From the rewritten code, collect every function whose
   name ≠ `main` and list them verbatim.

Output format (STRICT)
----------------------
Your entire response must consist ONLY of the following blocks, in this order:

[ANALYSIS]
<your chain‑of‑thought reasoning lives here>
[/ANALYSIS]

** FUNCTION: <funcName1> **
```cpp
<full definition of funcName1>
```

** FUNCTION: <funcName2> **
```cpp
<full definition of funcName2>
```
(…repeat for every remaining function…)
"""


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

_MULTI_STAGE_USER_PROMPT_V2 = "Here is the code:\n```cpp\n{code}\n```"

_STRUCTURE_FEW_SHOT_CODE_1 = """
// example.cpp
#include <iostream>
// add two numbers
double add(double a, double b) {
    return a + b;
}
int main() { std::cout << add(2.0, 3.0); return 0; }
"""

_STRUCTURE_FEW_SHOT_RESPOND_1 = """
[ANALYSIS]
The translation unit defines one helper function `add` and a `main` routine that performs console I/O.  To satisfy Cadence Stratus requirements we discard `main` (because of `std::cout`) and keep `add` verbatim: it is side‑effect‑free, uses only primitive arithmetic, and therefore is fully synthesizable.
[/ANALYSIS]

** FUNCTION: add **
```cpp
double add(double a, double b) {
    return a + b;
}
```
"""

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

_STRUCTURE_FEW_SHOT_RESPOND_2 = """
[ANALYSIS]
The file offers three numerical helpers—calc_sum, calc_mean, and calc_variance—plus a main routine with console output. We eliminate main to remove I/O. All three helpers are pure, free of recursion, dynamic memory, and STL containers, so they are synthesizable by Cadence Stratus without modification.
[/ANALYSIS]

** FUNCTION: calc_sum **
```cpp
double calc_sum(const double* arr, size_t n) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i)
        s += arr[i];
    return s;
}
```

** FUNCTION: calc_mean **
```cpp
double calc_mean(const double* arr, size_t n) {
    return calc_sum(arr, n) / static_cast<double>(n);
}
```

** FUNCTION: calc_variance **
```cpp
double calc_variance(const double* arr, size_t n) {
    double mu = calc_mean(arr, n);
    double var = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double diff = arr[i] - mu;
        var += diff * diff;
    }
    return var / static_cast<double>(n);
}
```
"""


def _build_prompt(code: str) -> str:
    user_prompt = _MULTI_STAGE_USER_PROMPT_V2.format(code=code)

    if "qwen" in MODEL_NAME.lower():
        system_prompt = prompt._QWEN_SYSTEM_PROMPT_HEAD + _SYSTEM_PROMPT_V2
    else:
        system_prompt = _SYSTEM_PROMPT_V2

    messages = [
        {"role": "system", "content": system_prompt},
        {
            "role": "user",
            "content": _MULTI_STAGE_USER_PROMPT_V2.format(
                code=_STRUCTURE_FEW_SHOT_CODE_1
            ),
        },
        {"role": "assistant", "content": _STRUCTURE_FEW_SHOT_RESPOND_1},
        {
            "role": "user",
            "content": _MULTI_STAGE_USER_PROMPT_V2.format(
                code=_STRUCTURE_FEW_SHOT_CODE_2
            ),
        },
        {"role": "assistant", "content": _STRUCTURE_FEW_SHOT_RESPOND_2},
        {"role": "user", "content": user_prompt},
    ]

    return _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )


_BLOCK_PAT = re.compile(
    r"""
    (?:\*\*|//\s*===)\s*FUNCTION:\s*      # block header prefix
    ([A-Za-z_]\w*)                        # ① function name
    .*?                                   # header tail (=== or **)
    \n```(?:cpp)?\s*                      # opening fence  ```cpp
    (.*?)                                 # ② whole function body
    \s*```                                # closing fence  ```
    """,
    re.S | re.VERBOSE,
)


def _parse_output(raw: str) -> list[dict]:
    matches = _BLOCK_PAT.findall(raw)
    if not matches:
        raise ValueError(
            "LLM output did not contain any FUNCTION blocks.\n"
            "--- OUTPUT START ---\n" + raw + "\n--- OUTPUT END ---"
        )

    functions = []
    for fname, fcode in matches:
        fcode = fcode.strip()
        sig_line = fcode.splitlines()[0].strip()

        # crude return‑type extraction: everything before the function name
        rt_match = re.match(
            rf"(?:inline\s+|static\s+|constexpr\s+|virtual\s+)*"
            rf"([^\s][^()]*?)\s+\*?&?\s*{re.escape(fname)}\b",
            sig_line,
        )
        return_type = rt_match.group(1).strip() if rt_match else "void"

        functions.append(
            {
                "name": fname,
                "return_type": return_type,
                "code": fcode,
            }
        )

    return functions


def extract_functions(src_path: str | Path, *, max_tokens: int = 4096) -> list[dict]:
    code = Path(src_path).read_text(encoding="utf-8", errors="ignore")
    messages = _build_prompt(code)
    raw = _llm.generate(
        messages,
        max_new_tokens=max_tokens,
    ).strip()

    functions = _parse_output(raw)

    return functions


def extract_functions_batch(
    code_strings: List[str],
    *,
    max_new_tokens: int = 4096,
    temperature: float = 0.0,
) -> List[List[dict]]:

    # 1) build prompts
    prompts = [_build_prompt(code) for code in code_strings]

    # 2) 一次丟進 vLLM 批次 API
    raw_outputs = _llm.generate_batch(
        prompts,
        max_new_tokens=max_new_tokens,
        temperature=temperature,
    )

    # 3) 逐條解析
    results: List[List[dict]] = []
    for raw in raw_outputs:
        raw = raw.strip()
        funcs = _parse_output(raw)  # 直接沿用單題解析
        results.append(funcs)
    return results


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
