import json
import os
import re
import sys
from pathlib import Path
from utils import VLLMGenerator

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
MODEL_NAME = os.getenv("FUNC_AGENT_MODEL", "Qwen/Qwen2.5-Coder-14B-Instruct")
_llm = VLLMGenerator(MODEL_NAME)

# ---------------------------------------------------------------------------
# One-shot in-context example without instruction header block
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT = "You are Qwen, created by Alibaba Cloud. You are a senior SystemC/Stratus engineer.\n"

# _FORMAT_PROMPT = 'Please output the result as a JSON array of objects, each object having "name" and "code" fields.\n'

_FORMAT_PROMPT_WITH_FUNC = 'The DUT has already been implemented. Please generate the testbench as a JSON array containing exactly two objects. The first object must have "name": "Testbench.cpp" and the second "name": "Testbench.h". Each object must also contain a "code" field with the corresponding SystemC source code.\n'

_REQUIREMENT_WITH_DUT = (
    "Given the SystemC DUT code below, generate the corresponding testbench code."
)
_FORMAT_PROMPT_WITH_DUT = 'Please output the result as a JSON array containing exactly two objects. The first object must have "name": "Testbench.cpp" and the second "name": "Testbench.h". Each object must also contain a "code" field with the corresponding SystemC source code.\n'

# Example input function and its Testbench.cpp, Testbench.h outputs
_EXAMPLE_REQUIREMENT = "Given the C++ program below, convert it into a functionally equivalent SystemC code. The expected input consists of two integer numbers."
_EXAMPLE_FUNC = "int add(int a, int b) { return a + b; }"
_EXAMPLE_DUT_CPP = """
#include \\"Dut.h\\"

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

#endif
"""
_EXAMPLE_TESTBENCH_CPP = """
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include \\"Testbench.h\\"

/* === Fixed Format === */
Testbench::Testbench(sc_module_name n)
    : sc_module(n) {
    SC_THREAD(do_feed);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(do_fetch);
    sensitive << i_clk.pos();
    dont_initialize();
}
/* === Fixed Format End === */

void Testbench::do_feed() {
    struct Testcase { int a, b; };
    std::vector<Testcase> tests;
    std::ifstream fin(\\"testcases.txt\\");
    if (!fin.is_open()) {
        std::cerr << \\"Error: Unable to open testcases.txt\\n\\";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == \\'#\\') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        Testcase tc;
        if (!(iss >> tc.a >> tc.b)) {
            std::cerr << \\"Warning: Incorrect format, skipping line: \\" << line << \\"\\n\\";
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        /* === Variable Section === */
        int a = tests[idx].a;
        int b = tests[idx].b;
        /* === Variable Section End === */

        /* === Variable Section === */
        o_a.write(a);
        o_b.write(b);
        /* === Variable Section End === */

        wait();
    }
}


void Testbench::do_fetch() {
    struct Testcase  { int a, b; };
    struct Golden { int expected; };

    std::vector<Testcase> tests;
    std::ifstream fin(\\"testcases.txt\\");
    if (!fin.is_open()) {
        std::cerr << \\"Error: Unable to open testcases.txt\\n\\";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == \\'#\\') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        Testcase tc;
        if (!(iss >> tc.a >> tc.b)) {
            std::cerr << \\"Warning: Incorrect format in testcases.txt, skipping line: \\" << line << \\"\\n\\";
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<Golden> goldens;
    {
        std::ifstream fin(\\"golden.txt\\");
        if (!fin.is_open()) {
            std::cerr << \\"Error: Unable to open golden.txt\\n\\";
            sc_stop();
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty() || line[0] == \\'#\\') continue;
            std::istringstream iss(line);
            Golden g;
            if (!(iss >> g.expected)) {
                std::cerr << \\"Warning: Incorrect format in golden.txt, skip: \\" << line << \\"\\n\\";
                continue;
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int result;

        result = i_result.read();

        bool passed = (result == goldens[idx].expected);

        if (passed) {
            std::cout << \\"Test case \\" << idx + 1 << \\" passed.\\n\\";
            std::cout << \\"Input: a = \\" << tests[idx].a << \\", b = \\" << tests[idx].b << \\"\\n\\";
            std::cout << \\"Output: \\" << result << \\"\\n\\n\\";
        } else {
            std::cerr << \\"Test case \\" << idx + 1 << \\" failed.\\n\\";
            std::cerr << \\"Input: a = \\" << tests[idx].a << \\", b = \\" << tests[idx].b << \\"\\n\\";
            std::cerr << \\"Output: \\" << result << \\", Expected: \\" << goldens[idx].expected << \\"\\n\\n\\";
            all_passed = false;
        }
    }
    /* === Variable Section End === */

    if (all_passed) {
        std::cout << \\"All tests passed successfully.\\" << std::endl;
    }
    else {
        SC_REPORT_FATAL(\\"Testbench\\", \\"Some test cases failed.\\");
    }
    sc_stop();
}
"""
_EXAMPLE_TESTBENCH_H = """
#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
using namespace sc_core;

class Testbench : public sc_module {
public:
  sc_in_clk i_clk;
  sc_out<bool> o_rst;

/* === Variable Section === */
  sc_fifo_out<int> o_a;
  sc_fifo_out<int> o_b;
  sc_fifo_in<int> i_result;
/* === Variable Section End === */

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench() = default;

private:

  void do_feed();
  void do_fetch();

};

#endif
"""


# ---------------------------------------------------------------------------
# Generate Testbench.cpp, Testbench.h via one-shot in-context learning
# ---------------------------------------------------------------------------
def generate_tb(
    func_code: str = "", dut_cpp: str = "", dut_h: str = "", requirement: str = ""
) -> dict[str, str]:
    if func_code:
        messages = [
            {"role": "system", "content": _SYSTEM_PROMPT},
            {
                "role": "user",
                "content": f"[Requirement]\n{_EXAMPLE_REQUIREMENT}\n{_FORMAT_PROMPT_WITH_FUNC}\n```cpp\n{_EXAMPLE_FUNC}\n```",
            },
            {
                "role": "assistant",
                "content": json.dumps(
                    [
                        {"name": "Testbench.cpp", "code": _EXAMPLE_TESTBENCH_CPP},
                        {"name": "Testbench.h", "code": _EXAMPLE_TESTBENCH_H},
                    ],
                    ensure_ascii=False,
                    indent=2,
                ),
            },
            {
                "role": "user",
                "content": (
                    (f"[Requirement]\n{requirement}" if requirement else "")
                    + f"\n{_FORMAT_PROMPT_WITH_FUNC}"
                    + "\n```cpp\n"
                    + func_code.strip()
                    + "\n```"
                ),
            },
        ]
    elif dut_cpp and dut_h:
        messages = [
            {"role": "system", "content": _SYSTEM_PROMPT},
            {
                "role": "user",
                "content": f"[Requirement]\n{_REQUIREMENT_WITH_DUT}\n{_FORMAT_PROMPT_WITH_DUT}\n[Dut.cpp]\n```cpp\n{_EXAMPLE_DUT_CPP}\n```\n[Dut.h]\n```cpp\n{_EXAMPLE_DUT_H}\n```",
            },
            {
                "role": "assistant",
                "content": json.dumps(
                    [
                        {"name": "Testbench.cpp", "code": _EXAMPLE_TESTBENCH_CPP},
                        {"name": "Testbench.h", "code": _EXAMPLE_TESTBENCH_H},
                    ],
                    ensure_ascii=False,
                    indent=2,
                ),
            },
            {
                "role": "user",
                "content": (
                    (f"[Requirement]\n{_REQUIREMENT_WITH_DUT}")
                    + f"\n{_FORMAT_PROMPT_WITH_DUT}"
                    + "\n[Dut.cpp]\n```cpp\n"
                    + dut_cpp.strip()
                    + "\n```"
                    + "\n[Dut.h]\n```cpp\n"
                    + dut_h.strip()
                    + "\n```"
                ),
            },
        ]
    else:
        raise ValueError("Either func_code or both dut_cpp and dut_h must be provided.")
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
    for req in ("Testbench.cpp", "Testbench.h"):
        if req not in file_map:
            raise RuntimeError(f"Missing '{req}' in model output\nRaw output:\n{raw}")

    return file_map


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) not in {2, 3}:
        sys.stderr.write("Usage: python tb_agent.py <func.json> [function_name]\n")
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

    tb_files = generate_tb(func_code)

    for fname, code in tb_files.items():
        Path(fname).write_text(code, encoding="utf-8")

    sys.stdout.write(tb_files["Testbench.cpp"])
