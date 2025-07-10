from __future__ import annotations

"""Generate SystemPipeline.cpp/h from Dut.h + Testbench.h via in‑context LLM prompting.

Usage (CLI):
    python pipe_agent.py path/to/Dut.h path/to/Testbench.h

The script sends Dut.h/Testbench.h to an LLM (via utils.VLLMGenerator)
with a one‑shot demonstration so the model returns a JSON list:
[
  {"name": "SystemPipeline.cpp", "code": "..."},
  {"name": "SystemPipeline.h",  "code": "..."}
]
Each file is then written to disk.
"""

import json
import os
import re
import sys
from pathlib import Path
from utils import VLLMGenerator

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
MODEL_NAME = os.getenv("PIPE_AGENT_MODEL", "Qwen/Qwen2.5-Coder-7B-Instruct")
_llm = VLLMGenerator(MODEL_NAME)

# ---------------------------------------------------------------------------
# One‑shot in‑context example (Dut.h + Testbench.h -> SystemPipeline.*)
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT = (
    "You are Qwen, created by Alibaba Cloud. You are a senior SystemC/Stratus "
    "integration engineer. Given exactly one Dut.h and one Testbench.h, "
    "produce a SystemPipeline.cpp and SystemPipeline.h"
    # "  that:\n"
    # "  • Instantiates a Testbench named 'tb' and a Dut named 'dut'.\n"
    # "  • Instantiates a sc_clock 'clk' (period CLOCK_PERIOD) and sc_signal<bool> 'rst'.\n"
    # "  • Connects clock/reset ports as in the canonical template.\n"
    # "  • Creates one sc_fifo signal for every matching (o_*/i_*) port pair.\n"
    # "    A matching pair is defined by identical base names after removing the prefix\n"
    # "    ('i_' vs 'o_') and identical element types.\n"
    "Output STRICTLY a JSON array of objects, each with keys 'name' and 'code'."
)

_EXAMPLE_DUT_H = """#ifndef DUT_H_\n#define DUT_H_\n#include <systemc>\nusing namespace sc_core;\nclass Dut: public sc_module {\npublic:\n  sc_in_clk  i_clk;\n  sc_in<bool> i_rst;\n  sc_fifo_in<int>  i_a;\n  sc_fifo_in<int>  i_b;\n  sc_fifo_out<int> o_result;\n  SC_HAS_PROCESS(Dut);\n  Dut(sc_module_name n);\n};\n#endif\n"""

_EXAMPLE_TB_H = """#ifndef TESTBENCH_H_\n#define TESTBENCH_H_\n#include <systemc>\nusing namespace sc_core;\nclass Testbench: public sc_module {\npublic:\n  sc_in_clk  i_clk;\n  sc_out<bool> o_rst;\n  sc_fifo_out<int> o_a;\n  sc_fifo_out<int> o_b;\n  sc_fifo_in<int>  i_result;\n  SC_HAS_PROCESS(Testbench);\n  Testbench(sc_module_name n);\n};\n#endif\n"""

_EXAMPLE_PIPE_CPP = """#include \"SystemPipeline.h\"\nSystemPipeline::SystemPipeline( sc_module_name n ): sc_module( n ),\n    tb(\"tb\"), dut(\"dut\"),\n    clk(\"clk\", CLOCK_PERIOD, SC_NS), rst(\"rst\")\n{\n    /* === Fixed Format === */\n    tb.i_clk(clk);\n    tb.o_rst(rst);\n    dut.i_clk(clk);\n    dut.i_rst(rst);\n    /* === Fixed Format End === */\n\n    /* === Variable Section === */\n    tb.o_a(channel_a);\n    tb.o_b(channel_b);\n    tb.i_result(channel_result);\n\n    dut.i_a(channel_a);\n    dut.i_b(channel_b);\n    dut.o_result(channel_result);\n    /* === Variable Section End === */\n}\n"""

_EXAMPLE_PIPE_H = """#ifndef SYSTEM_PIPELINE_H_\n#define SYSTEM_PIPELINE_H_\n#include <systemc>\nusing namespace sc_core;\n#include \"Testbench.h\"\n#include \"Dut.h\"\nclass SystemPipeline: public sc_module {\npublic:\n  SC_HAS_PROCESS(SystemPipeline);\n  SystemPipeline(sc_module_name n);\n  ~SystemPipeline() = default;\nprivate:\n  Testbench tb;\n  Dut       dut;\n  sc_clock  clk;\n  sc_signal<bool> rst;\n  sc_fifo<int> channel_a;\n  sc_fifo<int> channel_b;\n  sc_fifo<int> channel_result;\n};\n#endif\n"""

# ---------------------------------------------------------------------------
# Pipeline generation helper
# ---------------------------------------------------------------------------


def _build_messages(dut_h: str, tb_h: str) -> list[dict[str, str]]:
    """Craft chat messages for in‑context learning."""
    return [
        {"role": "system", "content": _SYSTEM_PROMPT},
        {
            "role": "user",
            "content": "Example Dut.h:\n```cpp\n" + _EXAMPLE_DUT_H + "\n```",
        },
        {
            "role": "user",
            "content": "Example Testbench.h:\n```cpp\n" + _EXAMPLE_TB_H + "\n```",
        },
        {
            "role": "assistant",
            "content": json.dumps(
                [
                    {"name": "SystemPipeline.cpp", "code": _EXAMPLE_PIPE_CPP},
                    {"name": "SystemPipeline.h", "code": _EXAMPLE_PIPE_H},
                ],
                ensure_ascii=False,
                indent=2,
            ),
        },
        {
            "role": "user",
            "content": (
                "Now generate SystemPipeline.cpp and SystemPipeline.h for the following files:\n"
                "--- Dut.h ---\n```cpp\n" + dut_h.strip() + "\n```\n"
                "--- Testbench.h ---\n```cpp\n" + tb_h.strip() + "\n```"
            ),
        },
    ]


def generate_pipeline(dut_h_code: str, testbench_h_code: str) -> dict[str, str]:
    """Return {file_name: code}. Raises on malformed LLM output."""
    messages = _build_messages(dut_h_code, testbench_h_code)
    prompt = _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )
    raw = _llm.generate(prompt, temperature=0.0).strip()

    match = re.search(r"\[.*\]", raw, re.S)
    if not match:
        raise ValueError(
            "LLM output missing JSON array.\n--- OUTPUT START ---\n"
            + raw
            + "\n--- OUTPUT END ---"
        )

    try:
        file_list = json.loads(match.group(0))
    except json.JSONDecodeError as e:
        raise ValueError(f"Failed to parse JSON: {e}\nRaw output:\n{raw}")

    file_map = {f["name"]: f["code"] for f in file_list if "name" in f and "code" in f}
    for req in ("SystemPipeline.cpp", "SystemPipeline.h"):
        if req not in file_map:
            raise RuntimeError(f"Missing '{req}' in model output")
    return file_map


# ---------------------------------------------------------------------------
# CLI entry point
# ---------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.stderr.write("Usage: python pipe_agent.py <Dut.h> <Testbench.h>\n")
        sys.exit(1)

    dut_path = Path(sys.argv[1])
    tb_path = Path(sys.argv[2])
    if not dut_path.exists() or not tb_path.exists():
        sys.stderr.write("Input file(s) do not exist.\n")
        sys.exit(1)

    dut_h_code = dut_path.read_text(encoding="utf-8")
    testbench_h_code = tb_path.read_text(encoding="utf-8")

    pipeline_files = generate_pipeline(dut_h_code, testbench_h_code)

    # write to current dir
    for fname, code in pipeline_files.items():
        Path(fname).write_text(code, encoding="utf-8")
        print(f"[pipe_agent] Wrote {fname}")

    # Print cpp to stdout for convenience
    sys.stdout.write(pipeline_files["SystemPipeline.cpp"])
