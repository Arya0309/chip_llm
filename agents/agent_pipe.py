from __future__ import annotations

import json
import os
import re
import sys
from pathlib import Path
from utils import DEFAULT_MODEL, VLLMGenerator

import prompts as prompt

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
MODEL_NAME = os.getenv("LLM_MODEL", DEFAULT_MODEL)
_llm = VLLMGenerator(MODEL_NAME)

# ---------------------------------------------------------------------------
# One‑shot in‑context example (Dut.h + Testbench.h -> SystemPipeline.*)
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT = "You are Qwen, created by Alibaba Cloud. You are a senior SystemC/Stratus integration engineer."

_SYSTEM_PROMPT_V2 = """
Role
----
• Senior SystemC integration engineer.  
• Connect an existing `Dut` and `Testbench` into a top-level module **SystemPipeline**.

Integration Rules
-----------------
1. Instantiate exactly ONE `Testbench tb` and ONE `Dut dut`.  
2. Instantiate
   ```cpp
   sc_clock  clk("clk", CLOCK_PERIOD, SC_NS);
   sc_signal<bool> rst("rst");
   ```
3. For every matching FIFO port pair (e.g. tb.o_a → dut.i_a) declare one sc_fifo<elem_t> channel_*; and bind as in the sample.
4. Keep fixed comment blocks intact; user should be able to compile directly.
5. No extra components, no fancy tracing.

Output format (STRICT)
----------------------
Your entire response must consist ONLY of the following blocks, in this order:

[ANALYSIS]
<your chain‑of‑thought reasoning lives here>
[/ANALYSIS]

** FILE: SystemPipeline.cpp **
```cpp
<full definition of Dut.cpp>
```

** FILE: SystemPipeline.h **
```cpp
<full definition of Dut.h>
```
"""

_OUTPUT_FORMAT = """
** FILE: SystemPipeline.cpp **
```cpp
{SYSTEM_PIPELINE_CPP}
```

** FILE: SystemPipeline.h **
```cpp
{SYSTEM_PIPELINE_H}
```
"""

_USER_PROMPT = "Given the following Dut.h and Testbench.h, generate SystemPipeline.cpp and SystemPipeline.h.\n"


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

    if "qwen" in MODEL_NAME.lower():
        system_prompt = prompt._QWEN_SYSTEM_PROMPT_HEAD + _SYSTEM_PROMPT_V2
    else:
        system_prompt = _SYSTEM_PROMPT_V2

    messages = [
        {"role": "system", "content": system_prompt},
        {
            "role": "user",
            "content": f"{_USER_PROMPT}--- Dut.h ---\n```cpp\n{_EXAMPLE_DUT_H}\n```\n--- Testbench.h ---\n```cpp\n{_EXAMPLE_TB_H}\n```",
        },
        {
            "role": "assistant",
            "content": _EXAMPLE_PIPE_CPP.format(
                SYSTEM_PIPELINE_CPP=_EXAMPLE_PIPE_CPP, SYSTEM_PIPELINE_H=_EXAMPLE_PIPE_H
            ),
        },
        {
            "role": "user",
            "content": f"{_USER_PROMPT}--- Dut.h ---\n```cpp\n{dut_h_code}\n```\n--- Testbench.h ---\n```cpp\n{testbench_h_code}\n```",
        },
    ]
    prompt = _llm.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )
    raw = _llm.generate(prompt, temperature=0.0).strip()

    # ------------------------------------------------------------
    # ❶  Find every "** FILE: <name> ** … ```cpp … ```" block
    # ------------------------------------------------------------
    block_pat = re.compile(
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

    matches = block_pat.findall(raw)    #  <--  no stripping of [ANALYSIS]
    if not matches:
        raise ValueError(
            "LLM output did not contain any FILE blocks.\n"
            "--- OUTPUT START ---\n" + raw + "\n--- OUTPUT END ---"
        )

    # ------------------------------------------------------------
    # ❷  Build the original {filename: code} mapping
    # ------------------------------------------------------------
    file_map = {fname.strip(): code.strip() for fname, code in matches}

    for req in ("SystemPipeline.cpp", "SystemPipeline.h"):
        if req not in file_map:
            raise RuntimeError(
                f"Missing '{req}' in model output\n--- OUTPUT START ---\n"
                + raw
                + "\n--- OUTPUT END ---"
            )

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
