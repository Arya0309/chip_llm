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
# One-shot in-context example without instruction header block
# ---------------------------------------------------------------------------
_SYSTEM_PROMPT = "You are Qwen, created by Alibaba Cloud. You are a senior SystemC/Stratus engineer.\n"

_SYSTEM_PROMPT_V2 = """
Role
----
• Senior SystemC verification engineer.  
• Generate a **self-checking Testbench** for the given DUT *or* for an original
  C++ function (when the DUT is yet to be generated).

Behaviour
---------
1. Provide clock/reset using the fixed thread template shown in the examples.  
2. Read `testcases.txt`, push inputs through the SAME FIFO ports declared in `Dut.h`; one write per datum (arrays looped).  
3. Read `golden.txt`, compare against `i_result`, print PASS/FAIL, call `SC_REPORT_FATAL` if any case fails, then `sc_stop()`.

Constraints & Style
-------------------
• Follow the same port names, types, and directions seen in the DUT.  
• Use blocking `read()` / `write()` only.  

Output format (STRICT)
----------------------
Your entire response must consist ONLY of the following blocks, in this order:

[ANALYSIS]
<your chain‑of‑thought reasoning lives here>
[/ANALYSIS]

** FILE: Testbench.cpp **
```cpp
<full definition of Testbench.cpp>
```

** FILE: Testbench.h **
```cpp
<full definition of Testbench.h>
```
"""

_OUTPUT_FORMAT = """
** FILE: Testbench.cpp **
```cpp
{Testbench_cpp}
```

** FILE: Testbench.h **
```cpp
{Testbench_h}
```
"""

_FORMAT_PROMPT_WITH_FUNC = 'The DUT has already been implemented. Please generate the testbench as a JSON array containing exactly two objects. The first object must have "name": "Testbench.cpp" and the second "name": "Testbench.h". Each object must also contain a "code" field with the corresponding SystemC source code.\n'

_REQUIREMENT_WITH_DUT = (
    "Given the SystemC DUT code below, generate the corresponding testbench code."
)
_FORMAT_PROMPT_WITH_DUT = 'Please output the result as a JSON array containing exactly two objects. The first object must have "name": "Testbench.cpp" and the second "name": "Testbench.h". Each object must also contain a "code" field with the corresponding SystemC source code.\n'

# Example input function and its Testbench.cpp, Testbench.h outputs
_EXAMPLE_REQUIREMENT_1 = "Given the C++ program below, convert it into a functionally equivalent SystemC code. The expected input consists of two integer numbers."
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

#endif
"""
_EXAMPLE_TESTBENCH_CPP_1 = r"""
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Testbench.h"

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
    std::ifstream fin("testcases.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt\n";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        Testcase tc;
        if (!(iss >> tc.a >> tc.b)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
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
    std::ifstream fin("testcases.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt\n";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        Testcase tc;
        if (!(iss >> tc.a >> tc.b)) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<Golden> goldens;
    {
        std::ifstream fin("golden.txt");
        if (!fin.is_open()) {
            std::cerr << "Error: Unable to open golden.txt\n";
            sc_stop();
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            Golden g;
            if (!(iss >> g.expected)) {
                std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << "\n";
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
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: a = " << tests[idx].a << ", b = " << tests[idx].b << "\n";
            std::cout << "Output: " << result << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: a = " << tests[idx].a << ", b = " << tests[idx].b << "\n";
            std::cerr << "Output: " << result << ", Expected: " << goldens[idx].expected << "\n\n";
            all_passed = false;
        }
    }
    /* === Variable Section End === */

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    }
    else {
        SC_REPORT_FATAL("Testbench", "Assertion failed.");
    }
    sc_stop();
}
"""

_EXAMPLE_TESTBENCH_H_1 = """
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

_EXAMPLE_REQUIREMENT_2 = "Given the C++ program below, convert it into a functionally equivalent SystemC code. The expected input consists of two integer array."

_EXAMPLE_FUNC_2 = """
void add_arrays(const int A[N], const int B[N], int C[N]) {
    for (int i = 0; i < N; ++i) C[i] = A[i] + B[i];
}
"""

_EXAMPLE_DUT_CPP_2 = """
#include "Dut.h"

constexpr int N = 4;

void add_arrays(const int A[N], const int B[N], int C[N]) {
    for (int i = 0; i < N; ++i) C[i] = A[i] + B[i];
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

    {
        wait();
    }

    while (true) {

        int a[N];
        int b[N];
        int c[N];

        for (int i = 0; i < N; i++) {
            a[i] = i_a.read(); // Read from input FIFO
            b[i] = i_b.read(); // Read from input FIFO
        }
        /* === Variable Section End === */

        /* === Main Function ===*/
        add_arrays(a, b, c);
        /* === Main Function End === */

        /* === Variable Section === */
        for (int i = 0; i < N; ++i) {
            o_c.write(c[i]);
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
  sc_fifo_out<int> o_c;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
#endif
"""

_EXAMPLE_TESTBENCH_CPP_2 = r"""
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Testbench.h"

constexpr int N = 4;

/* === Fixed Format === */
Testbench::Testbench(sc_module_name n) : sc_module(n) {
    SC_THREAD(do_feed);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(do_fetch);
    sensitive << i_clk.pos();
    dont_initialize();
}
/* === Fixed Format End === */

void Testbench::do_feed() {
    struct Testcase { std::vector<int> A, B; };
    std::vector<Testcase> tests;

    std::ifstream fin("testcases.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt\n";
        sc_stop();
        return;
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);

        std::vector<int> tokens;
        int v;
        while (iss >> v) tokens.push_back(v);
        if (tokens.size() != 2 * N) {
            std::cerr << "Warning: Skip line with incorrect token count: "
                      << line << '\n';
            continue;
        }
        Testcase tc;
        tc.A.assign(tokens.begin(),             tokens.begin() + N);
        tc.B.assign(tokens.begin() + N,          tokens.end());
        tests.push_back(std::move(tc));
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto &tc : tests) {
        for (int i = 0; i < N; ++i) o_a.write(tc.A[i]);
        for (int i = 0; i < N; ++i) o_b.write(tc.B[i]);
        wait();
    }
}

void Testbench::do_fetch() {
    struct Testcase { std::vector<int> A, B; };
    struct Golden { std::vector<int> C; };

    std::vector<Testcase> tests;

    std::ifstream fin("testcases.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt\n";
        sc_stop();
        return;
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);

        std::vector<int> tokens;
        int v;
        while (iss >> v) tokens.push_back(v);
        if (tokens.size() != 2 * N) {
            std::cerr << "Warning: Skip line with incorrect token count: "
                      << line << '\n';
            continue;
        }
        Testcase tc;
        tc.A.assign(tokens.begin(),             tokens.begin() + N);
        tc.B.assign(tokens.begin() + N,          tokens.end());
        tests.push_back(std::move(tc));
    }
    fin.close();

    std::vector<Golden> goldens;

    std::ifstream gin("golden.txt");
    if (!gin.is_open()) {
        std::cerr << "Error: Unable to open golden.txt\n";
        sc_stop();
        return;
    }

    while (std::getline(gin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);

        std::vector<int> vals;
        int v;
        while (iss >> v) vals.push_back(v);
        if (vals.size() != N) {
            std::cerr << "Warning: Skip golden line with incorrect length: "
                      << line << '\n';
            continue;
        }
        goldens.push_back({std::move(vals)});
    }
    gin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        std::vector<int> result;
        result.reserve(N);
        for (int i = 0; i < N; ++i) {
            int x = i_c.read();
            result.push_back(x);
        }

        bool passed = (result == goldens[idx].C);
        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
        } else {
            all_passed = false;
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: A = [";
            for (int i = 0; i < N; ++i) {
                std::cerr << tests[idx].A[i] << (i < N - 1 ? ", " : "");
            }
            std::cerr << "], B = [";
            for (int i = 0; i < N; ++i) {
                std::cerr << tests[idx].B[i] << (i < N - 1 ? ", " : "");
            }
            std::cerr << "]\n";
            std::cerr << "Output: ";
            for (int x : result)        std::cerr << x << ' ';
            std::cerr << "\nExpected: ";
            for (int x : goldens[idx].C) std::cerr << x << ' ';
            std::cerr << '\n';
        }
    }

    if (!all_passed) {
        SC_REPORT_FATAL("Testbench", "Assertion failed.");
    }
    sc_stop();
}
"""


_EXAMPLE_TESTBENCH_H_2 = """
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
  sc_fifo_in<int> i_c;
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

    if "qwen" in MODEL_NAME.lower():
        system_prompt = prompt._QWEN_SYSTEM_PROMPT_HEAD + _SYSTEM_PROMPT_V2
    else:
        system_prompt = _SYSTEM_PROMPT_V2

    if func_code:
        messages = [
            {"role": "system", "content": system_prompt},
            {
                "role": "user",
                "content": f"[Requirement]\n{_EXAMPLE_REQUIREMENT_1}\n```cpp\n{_EXAMPLE_FUNC_1}\n```",
            },
            {
                "role": "assistant",
                "content": _OUTPUT_FORMAT.format(
                    Testbench_cpp=_EXAMPLE_TESTBENCH_CPP_1,
                    Testbench_h=_EXAMPLE_TESTBENCH_H_1,
                ),
            },
            {
                "role": "user",
                "content": (
                    (f"[Requirement]\n{requirement}" if requirement else "")
                    + "\n```cpp\n"
                    + func_code.strip()
                    + "\n```"
                ),
            },
        ]
    elif dut_cpp and dut_h:
        messages = [
            {"role": "system", "content": _SYSTEM_PROMPT_V2},
            {
                "role": "user",
                "content": f"[Requirement]\n{_REQUIREMENT_WITH_DUT}\n[Dut.cpp]\n```cpp\n{_EXAMPLE_DUT_CPP_1}\n```\n[Dut.h]\n```cpp\n{_EXAMPLE_DUT_H_1}\n```",
            },
            {
                "role": "assistant",
                "content": _OUTPUT_FORMAT.format(
                    Testbench_cpp=_EXAMPLE_TESTBENCH_CPP_1,
                    Testbench_h=_EXAMPLE_TESTBENCH_H_1,
                ),
            },
            {
                "role": "user",
                "content": f"[Requirement]\n{_REQUIREMENT_WITH_DUT}\n[Dut.cpp]\n```cpp\n{_EXAMPLE_DUT_CPP_2}\n```\n[Dut.h]\n```cpp\n{_EXAMPLE_DUT_H_2}\n```",
            },
            {
                "role": "assistant",
                "content": _OUTPUT_FORMAT.format(
                    Testbench_cpp=_EXAMPLE_TESTBENCH_CPP_2,
                    Testbench_h=_EXAMPLE_TESTBENCH_H_2,
                ),
            },
            {
                "role": "user",
                "content": (
                    (f"[Requirement]\n{_REQUIREMENT_WITH_DUT}")
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
    raw = _llm.generate(formatted).strip()

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

    matches = block_pat.findall(raw)  #  <--  no stripping of [ANALYSIS]
    if not matches:
        raise ValueError(
            "LLM output did not contain any FILE blocks.\n"
            "--- OUTPUT START ---\n" + raw + "\n--- OUTPUT END ---"
        )

    # ------------------------------------------------------------
    # ❷  Build the original {filename: code} mapping
    # ------------------------------------------------------------
    file_map = {fname.strip(): code.strip() for fname, code in matches}

    for req in ("Testbench.cpp", "Testbench.h"):
        if req not in file_map:
            raise RuntimeError(
                f"Missing '{req}' in model output\n--- OUTPUT START ---\n"
                + raw
                + "\n--- OUTPUT END ---"
            )

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
