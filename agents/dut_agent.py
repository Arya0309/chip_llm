from __future__ import annotations
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
# One-shot in-context example without instruction header block
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT = "You are Qwen, created by Alibaba Cloud. You are a senior SystemC/Stratus engineer.\n"

_FORMAT_PROMPT = 'Please output the result as a JSON array of objects, each object having "name" and "code" fields.\n'

# Example input function and its Dut.cpp output
_EXAMPLE_FUNC = "int add(int a, int b) { return a + b; }"
_EXAMPLE_DUT_CPP = """
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

_EXAMPLE_DUT_H = """
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
"""


# ---------------------------------------------------------------------------
# Generate Dut.cpp via one-shot in-context learning
# ---------------------------------------------------------------------------
def generate_dut(func_code: str, requirement: str = "") -> dict[str, str]:
    messages = [
        {"role": "system", "content": _SYSTEM_PROMPT},
        {"role": "user", "content": f"Example function:\n```cpp\n{_EXAMPLE_FUNC}\n```"},
        {
            "role": "assistant",
            "content": json.dumps(
                [
                    {"name": "Dut.cpp", "code": _EXAMPLE_DUT_CPP},
                    {"name": "Dut.h", "code": _EXAMPLE_DUT_H},
                ],
                ensure_ascii=False,
                indent=2,
            ),
        },
        {
            "role": "user",
            "content": (
                (f"[Requirement]\n{requirement}\n\n" if requirement else "")
                + "\n```cpp\n"
                + func_code.strip()
                + "\n```"
            ),
        },
    ]

    formatted = _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )
    raw = _llm.generate(formatted, temperature=0.0).strip()

    match = re.search(r"\[.*\]", raw, re.S)
    if not match:
        raise ValueError(
            "LLM output did not contain a JSON array.\n--- OUTPUT START ---\n"
            + raw
            + "\n--- OUTPUT END ---"
        )

    try:
        file_list = json.loads(match.group(0))
    except json.JSONDecodeError as e:
        raise ValueError(f"Failed to parse JSON: {e}\nRaw output:\n{raw}")

    file_map = {f["name"]: f["code"] for f in file_list if "name" in f and "code" in f}
    for req in ("Dut.cpp", "Dut.h"):
        if req not in file_map:
            raise RuntimeError(f"Missing '{req}' in model output")

    return file_map


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
