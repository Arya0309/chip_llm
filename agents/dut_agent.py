from __future__ import annotations
import json
import os
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
_SYSTEM_PROMPT = (
    "You are Qwen, created by Alibaba Cloud. You are a senior SystemC/Stratus engineer.\n"
    "Given exactly one C++ function, generate a complete Dut.cpp.\n"
    "Your output must include: \n"
    "  - The original function verbatim.\n"
    "  - The Dut constructor with: SC_THREAD(do_compute), sensitive << i_clk.pos(), dont_initialize(), reset_signal_is(i_rst, false).\n"
    "  - In do_compute():\n"
    "      * A Variable Section to read each input port: <type> <name> = i_<name>.read();\n"
    "      * A Main function Section that calls the function, assigning return to res if non-void.\n"
    "      * A Variable Section that writes the result: o_result.write(res); if return not void.\n"
)

# Example input function and its Dut.cpp output
_EXAMPLE_FUNC = "int add(int a, int b) { return a + b; }"
_EXAMPLE_DUT = """
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


# ---------------------------------------------------------------------------
# Generate Dut.cpp via one-shot in-context learning
# ---------------------------------------------------------------------------
def generate_dut(func_code: str) -> str:
    messages = [
        {"role": "system", "content": _SYSTEM_PROMPT},
        {"role": "user", "content": f"Example function:\n```cpp\n{_EXAMPLE_FUNC}\n```"},
        {"role": "assistant", "content": _EXAMPLE_DUT},
        {
            "role": "user",
            "content": f"Now generate Dut.cpp for this function:\n```cpp\n{func_code.strip()}\n```",
        },
    ]
    formatted = _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )
    raw = _llm.generate(formatted, temperature=0.0)
    # Return from first include
    idx = raw.find("#include")
    return raw[idx:].strip() if idx != -1 else raw.strip()


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

    target = sys.argv[2] if len(sys.argv) == 3 else entries[0]["name"]
    item = next((e for e in entries if e["name"] == target), None)
    if item is None:
        sys.stderr.write(f"Function '{target}' not found.\n")
        sys.exit(1)

    dut_cpp = generate_dut(item["code"])
    sys.stdout.write(dut_cpp)
