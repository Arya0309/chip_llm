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
_SYSTEM_PROMPT = "You are Qwen, created by Alibaba Cloud. You are a senior SystemC/Stratus integration engineer."

_USER_PROMPT = (
    "Given the following Dut.h and Testbench.h, generate SystemPipeline.cpp and SystemPipeline.h.\n"
    "--- Dut.h ---\n```cpp\n{dut_h}\n```\n"
    "--- Testbench.h ---\n```cpp\n{tb_h}\n```"
)

_EXAMPLE_DUT_H = """#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut: public sc_module {
public:
  sc_in_clk  i_clk;
  sc_in<bool> i_rst;

  sc_fifo_in<int>  i_a;
  sc_fifo_in<int>  i_b;
  sc_fifo_out<int> o_result;

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
#endif
"""

_EXAMPLE_TB_H = """#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
using namespace sc_core;
class Testbench: public sc_module {
public:
  sc_in_clk  i_clk;
  sc_out<bool> o_rst;

  sc_fifo_out<int> o_a;
  sc_fifo_out<int> o_b;
  sc_fifo_in<int>  i_result;

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench() = default;

private:

  void do_feed();
  void do_fetch();

};
#endif
"""

_EXAMPLE_PIPE_CPP = """#include "SystemPipeline.h"
SystemPipeline::SystemPipeline( sc_module_name n ): sc_module( n ),
    tb("tb"), dut("dut"),
    clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{
    /* === Fixed Format === */
    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);
    /* === Fixed Format End === */

    /* === Variable Section === */
    tb.o_a(channel_a);
    tb.o_b(channel_b);
    tb.i_result(channel_result);

    dut.i_a(channel_a);
    dut.i_b(channel_b);
    dut.o_result(channel_result);
    /* === Variable Section End === */
}
"""

_EXAMPLE_PIPE_H = """#ifndef SYSTEM_PIPELINE_H_
#define SYSTEM_PIPELINE_H_

#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#include "Dut.h"

class SystemPipeline: public sc_module {
public:
  SC_HAS_PROCESS(SystemPipeline);
  SystemPipeline(sc_module_name n);
  ~SystemPipeline() = default;
private:
  Testbench tb;
  Dut       dut;

  sc_clock  clk;
  sc_signal<bool> rst;

  sc_fifo<int> channel_a;
  sc_fifo<int> channel_b;
  sc_fifo<int> channel_result;
};
#endif
"""


# ---------------------------------------------------------------------------
# Pipeline generation helper
# --------------------------------------------------------------------------
def generate_pipeline(dut_h_code: str, testbench_h_code: str) -> dict[str, str]:
    """Return {file_name: code}. Raises on malformed LLM output."""
    messages = [
        {"role": "system", "content": _SYSTEM_PROMPT},
        {
            "role": "user",
            "content": _USER_PROMPT.format(
                dut_h=_EXAMPLE_DUT_H.strip(), tb_h=_EXAMPLE_TB_H.strip()
            ),
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
            "content": _USER_PROMPT.format(
                dut_h=dut_h_code.strip(), tb_h=testbench_h_code.strip()
            ),
        },
    ]
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
