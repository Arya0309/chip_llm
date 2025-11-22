from __future__ import annotations
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
from utils import LLMGeneratorFactory, DEFAULT_MODEL

MODEL_NAME = os.getenv("LLM_MODEL", DEFAULT_MODEL)
_llm = LLMGeneratorFactory(MODEL_NAME) # 讓工廠決定用哪一個 LLMGenerator

# ---------------------------------------------------------------------------
# One-shot in-context example without instruction header block
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT = "You are a senior SystemC/Stratus engineer.\n"

_SYSTEM_PROMPT_V2 = """
Role
----
• Senior SystemC / Cadence Stratus design engineer.  
• Convert user-supplied C/C++ function(s) into a **synthesizable DUT that uses FIFO streams ONLY** for all data communication.

Hard Rules
----------
1. Place functions at the very top of Dut.cpp, before the Dut class definition.
2. **Exactly one FIFO port per logical tensor** – arrays/matrices are serialised element-by-element; never declare `sc_fifo_in<T> foo[N]` or multi-dim FIFO.
3. Keep the fixed scaffold:
   ```cpp
   SC_THREAD(do_compute);
   sensitive << i_clk.pos();
   dont_initialize();
   reset_signal_is(i_rst, false);```
   These lines must remain unmodified.
4. **Blocking I/O ONLY**: Always use blocking `port.read()` (or `val = port.read()`) and `port.write(val)`. 
   - **NEVER use `nb_read()` or `nb_write()`**. 
   - In HLS `SC_THREAD`, we must WAIT for data to arrive. `nb_read` causes immediate failure if the FIFO is empty.

Output format (STRICT)
----------------------
Your entire response must consist ONLY of the following blocks, in this order:

[ANALYSIS]
<your chain‑of‑thought reasoning lives here>
[/ANALYSIS]
"""

# format
_QUERY_FORMAT = """
[Requirement]
{USER_PROMPT}
```cpp
{FUNC}
```
{GENERATE}
"""
_CODE_FORMAT = """
```cpp
{CODE}
```
"""

_GENERATE_H = "Please generate ** ONLY ** Dut.h."
_GENERATE_CPP = "Please generate ** ONLY ** DUT.cpp."

# Example input function and its Dut.cpp output
_EXAMPLE_USER_PROMPT_1 = "Given the C++ program below, convert it into a functionally equivalent SystemC code. The expected input consists of two integer numbers."
_EXAMPLE_FUNC_1 = "int add(int a, int b) { return a + b; }"
_EXAMPLE_DUT_CPP_1 = """
#include "Dut.h"

int add(int a, int b) { return a + b; }

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        int a = i_a.read();
        int b = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        int res = add(a, b);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
"""
_EXAMPLE_DUT_H_1 = """
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_a;
  sc_fifo_in<int> i_b;
  sc_fifo_out<int> o_result;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
#endif // DUT_H_
"""

# Another few-shot example: element-wise addition of two fixed-size arrays
_EXAMPLE_USER_PROMPT_2 = "Given the C++ program below, convert it into a functionally equivalent SystemC code. The expected input consists of two 4-element integer array."
_EXAMPLE_FUNC_2 = (
    "void add_arrays(const int A[N], const int B[N], int C[N]) { "
    "for (int i = 0; i < N; ++i) C[i] = A[i] + B[i]; }"
)
_EXAMPLE_DUT_CPP_2 = """
#include "Dut.h"

constexpr int N = 4;

// original array-addition function
void add_arrays(const int A[N], const int B[N], int C[N]) {
    for(int i = 0; i < N; ++i) {
        C[i] = A[i] + B[i];
    }
}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        int A[N];
        int B[N];
        // CRITICAL: Use blocking read() to wait for data. 
        // NEVER use nb_read() as it breaks HLS handshaking.
        for (int i = 0; i < N; ++i) {
            A[i] = i_a.read();
        }
        for (int i = 0; i < N; ++i) {
            B[i] = i_b.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        int C[N];
        add_arrays(A, B, C);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result array back out
        for (int i = 0; i < N; ++i) {
            o_result.write(C[i]);
        }
        /* === Variable Section End === */
    }
}
"""
_EXAMPLE_DUT_H_2 = """
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

  /* === Variable Section === */
  sc_fifo_in<int> i_a;
  sc_fifo_in<int> i_b;
  sc_fifo_out<int> o_result;
  /* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_
"""


def _build_prompt(
    func_code: str,
    requirement: str = "",
    system_prompt: str = _SYSTEM_PROMPT,
    response_h: str = None,
) -> str:
    if "qwen" in MODEL_NAME.lower():
        system_prompt = prompt._QWEN_SYSTEM_PROMPT_HEAD + _SYSTEM_PROMPT_V2

    messages = [
        {"role": "system", "content": system_prompt},
        # Few Shot - 1
        {
            "role": "user",
            "content": _QUERY_FORMAT.format(
                USER_PROMPT=_EXAMPLE_USER_PROMPT_1,
                FUNC=_EXAMPLE_FUNC_1,
                GENERATE=_GENERATE_H,
            ),
        },
        {
            "role": "assistant",
            "content": _CODE_FORMAT.format(CODE=_EXAMPLE_DUT_H_1),
        },
        {
            "role": "user",
            "content": _GENERATE_CPP,
        },
        {
            "role": "assistant",
            "content": _CODE_FORMAT.format(CODE=_EXAMPLE_DUT_CPP_1),
        },
        # Few Shot - 2
        {
            "role": "user",
            "content": _QUERY_FORMAT.format(
                USER_PROMPT=_EXAMPLE_USER_PROMPT_2,
                FUNC=_EXAMPLE_FUNC_2,
                GENERATE=_GENERATE_H,
            ),
        },
        {
            "role": "assistant",
            "content": _CODE_FORMAT.format(CODE=_EXAMPLE_DUT_H_2),
        },
        {
            "role": "user",
            "content": _GENERATE_CPP,
        },
        {
            "role": "assistant",
            "content": _CODE_FORMAT.format(CODE=_EXAMPLE_DUT_CPP_2),
        },
        # REAL QUERY
        {
            "role": "user",
            "content": (
                (f"[Requirement]\n{requirement}\n\n" if requirement else "")
                + "\n```cpp\n"
                + func_code.strip()
                + "\n```"
                + _GENERATE_H
            ),
        },
    ]

    if response_h is not None:
        messages.append({"role": "assistant", "content": response_h})
        messages.append({"role": "user", "content": _GENERATE_CPP})

    # 交給 vLLM 做 chat‑template，產生最終文字 prompt
    return {
        "prompt": _llm.apply_chat_template(
            messages,
            tokenize=False,
            add_generation_prompt=True,
        ),
        "messages": messages,
    }


_BLOCK_PAT = re.compile(
    r"```cpp\s*(.*?)\s*```",
    re.S | re.VERBOSE,
)


def _parse_output(raw_h: str, raw_cpp: str) -> Dict[str, str]:

    def extract(raw):
        matches = _BLOCK_PAT.findall(raw)
        if not matches:
            print(
                "LLM output did not contain any FILE blocks.\n"
                "--- OUTPUT START ---\n" + raw + "\n--- OUTPUT END ---"
            )
            return ""
        return matches[0].strip()

    code_h = extract(raw_h)
    code_cpp = extract(raw_cpp)
    file_map = {"Dut.h": code_h, "Dut.cpp": code_cpp}
    return file_map


_＿BLOCK_PAT = re.compile(
    r"""
    \*\*\s*FILE:\s*              # "** FILE:" header
    ([^*]+?)                     # ① filename   (lazy until next '*')
    \s*\*\*\s*                   # closing "**"
    \n```(?:[a-zA-Z0-9_+-]+)?\s* # opening ``` or ```cpp
    (.*?)                        # ② file body  (non‑greedy, DOTALL)
    \s*```                       # closing fence
    """,
    re.S | re.VERBOSE,
)


def generate_dut(
    func_code: str, requirement: str = "", system_prompt: str = _SYSTEM_PROMPT
) -> dict[str, str]:

    prompt = _build_prompt(func_code, requirement, system_prompt)
    response_h = _llm.generate(prompt).strip()
    prompt = _build_prompt(func_code, requirement, system_prompt, response_h)
    response_cpp = _llm.generate(prompt).strip()

    results = _parse_output(response_h.strip(), response_cpp.strip())
    return results


def generate_dut_batch(
    func_codes: List[str],
    requirement: List[str] | None = None,
    system_prompt: str = _SYSTEM_PROMPT_V2,
    *,
    temperature: float = 0.3,
    top_p: float = 0.8,
    max_new_tokens: int = 4096,
) -> List[Dict[str, str]]:

    if requirement is None:
        requirement = [""] * len(func_codes)
    elif len(requirement) != len(func_codes):
        raise ValueError("len(requirement) must equal len(func_codes)")

    print("[DUT] Generating DUT.h")
    ret = [
        _build_prompt(code, req, system_prompt)
        for code, req in zip(func_codes, requirement)
    ]
    prompts = [r["prompt"] for r in ret]
    responses_h = _llm.generate_batch(
        prompts,
        temperature=temperature,
        top_p=top_p,
        max_new_tokens=max_new_tokens,
    )

    print("[DUT] Generating DUT.cpp")
    ret = [
        _build_prompt(code, req, system_prompt, resp_h)
        for code, req, resp_h in zip(func_codes, requirement, responses_h)
    ]
    prompts = [r["prompt"] for r in ret]
    messages = [r["messages"] for r in ret]
    responses_cpp = _llm.generate_batch(
        prompts,
        temperature=temperature,
        top_p=top_p,
        max_new_tokens=max_new_tokens,
    )

    for msg, resp in zip(messages, responses_cpp):
        msg.append({"role": "assistant", "content": resp})

    # 3) 解析，每條輸出都跑原本 regex
    results: List[Dict[str, str]] = []
    for h, cpp in zip(responses_h, responses_cpp):
        try:
            results.append(_parse_output(h.strip(), cpp.strip()))
        except Exception as e:
            print(f"[generate_dut_batch] parse error: {e}")
            results.append({"Dut.h": "", "Dut.cpp": ""})
    return results, messages


# ────────────────────────────────────────────────────────────────
# 💡 新增：統一的 refine() 介面，供 agent_verifier.py 呼叫
# ----------------------------------------------------------------
def refine(
    messages: list[dict],
    extra_user_msg: str = "",
    *,
    max_new_tokens: int = 4096,
    temperature: float = 0.3,
    top_p: float = 0.8,
) -> dict[str, str]:
    """
    續接對話，重新產生檔案（單題單次，但仍走 batch 介面）。

    Parameters
    ----------
    messages : list[dict]
        完整 chat history（已包含剛 append 的 [ISSUE] 訊息）
    extra_user_msg : str
        目前已包含在 messages，可忽略
    max_new_tokens, temperature, top_p : float
        取樣超參；與 generate_*_batch 保持一致

    Returns
    -------
    dict[str, str]
        key = filename, value = source code
    """
    messages[-1]["content"] += "\n\n" + _GENERATE_H + "\n"

    # 1) 將 chat messages 轉成 vLLM 接受的 prompt 字串
    prompts = _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )

    # 2) 透過 batch API（雖然只有 1 條），帶自訂溫度等參數
    response_h = _llm.generate_batch(
        [prompts],
        max_new_tokens=max_new_tokens,
        temperature=temperature,
        top_p=top_p,
    )[0].strip()
    messages.append({"role": "assistant", "content": response_h})
    messages.append({"role": "user", "content": _GENERATE_CPP})

    # 1)
    prompts = _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )

    # 2)
    response_cpp = _llm.generate_batch(
        [prompts],
        max_new_tokens=max_new_tokens,
        temperature=temperature,
        top_p=top_p,
    )[0].strip()
    messages.append({"role": "assistant", "content": response_cpp})

    # 3) 解析輸出 → {filename: code}
    parse_fn_candidates = [
        globals().get("_parse_output"),
        globals().get("_parse_dut_output"),
        globals().get("_parse_tb_output"),
        globals().get("_parse_pipe_output"),
        globals().get("parse_output"),
    ]
    parse_fn = next((f for f in parse_fn_candidates if callable(f)), None)
    if parse_fn is None:
        raise RuntimeError("No parse_*_output() function found in this agent.")

    return parse_fn(response_h, response_cpp), messages


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) not in {2, 3}:
        sys.stderr.write("Usage: python dut_agent.py <func.json> [function_name]\n")
        sys.exit(1)

    json_path = Path(sys.argv[1])
    entries = json.loads(json_path.read_text(encoding="utf-8"))
    if not isinstance(entries, list):
        sys.stderr.write("func.json must be a list from func_agent\n")
        sys.exit(1)

    if len(sys.argv) == 3:
        target = sys.argv[2]
        item = next((e for e in entries if e["name"] == target), None)
        if item is None:
            sys.stderr.write(f"Function '{target}' not found.\n")
            sys.exit(1)
        func_code = item["code"]
        out_prefix = item["name"]
    else:
        func_code = "\n\n".join(e["code"] for e in entries)
        out_prefix = "combined"

    dut_files = generate_dut(func_code)

    for fname, code in dut_files.items():
        Path(fname).write_text(code, encoding="utf-8")

    sys.stdout.write(dut_files["Dut.cpp"])
