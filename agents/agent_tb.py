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
_llm = LLMGeneratorFactory(MODEL_NAME) # 讓工廠決定用哪一個

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
"""

_QUERY_FORMAT = """
[Requirement]
{REQUIREMENT_WITH_DUT}
[Dut.h]
```cpp
{DUT_H}
```
[Dut.cpp]
```cpp
{DUT_CPP}
```
{GENERATE}
"""
_CODE_FORMAT = """
```cpp
{CODE}
```
"""

_GENERATE_H = "Please generate ** ONLY ** Testbench.h."
_GENERATE_CPP = "Please generate ** ONLY ** Testbench.cpp."

_FORMAT_PROMPT_WITH_FUNC = 'The DUT has already been implemented. Please generate the testbench as a JSON array containing exactly two objects. The first object must have "name": "Testbench.cpp" and the second "name": "Testbench.h". Each object must also contain a "code" field with the corresponding SystemC source code.\n'

_DESCRIPTION_WITH_DUT = (
    "Given the SystemC DUT code below, generate the corresponding testbench code."
)
_FORMAT_PROMPT_WITH_DUT = 'Please output the result as a JSON array containing exactly two objects. The first object must have "name": "Testbench.cpp" and the second "name": "Testbench.h". Each object must also contain a "code" field with the corresponding SystemC source code.\n'

# Example input function and its Testbench.cpp, Testbench.h outputs
_EXAMPLE_REQUIREMENT_1 = "Given the C++ program below, convert it into a functionally equivalent SystemC code. The expected input consists of two integer numbers."
_EXAMPLE_REQUIREMENT_WITH_DUT_1 = """
Given the SystemC DUT code below, generate the corresponding testbench code.
Expected input consists of:
    two numbers
Expected output consists of:
    a number
"""
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
    std::ifstream gin("golden.txt");
    if (!gin.is_open()) {
        std::cerr << "Error: Unable to open golden.txt\n";
        sc_stop();
        return;
    }

    while (std::getline(gin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        Golden g;
        if (!(iss >> g.expected)) {
            std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << "\n";
            continue;
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
    gin.close();

    wait(1);

    size_t passed_count = 0;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int result;
        result = i_result.read();

        bool passed = (result == goldens[idx].expected);
        if (passed) ++passed_count;
    }
    /* === Variable Section End === */

    if (passed_count == goldens.size()) {
        std::cout << "All test cases passed!\n";
    } else {
        std::cerr << goldens.size() - passed_count << " out of " << goldens.size() << " test cases failed.\n";
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
_EXAMPLE_REQUIREMENT_WITH_DUT_2 = """
Given the SystemC DUT code below, generate the corresponding testbench code.
Expected input consists of:
    two 4-element array
Expected output consists of:
    a 4-element array
"""
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
        /* === Variable Section === */
        std::vector<int> vals;
        int v;
        while (iss >> v) vals.push_back(v);
        if (vals.size() != 2 * N) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << '\n';
            continue;
        }
        Testcase tc;
        tc.A.assign(vals.begin(), vals.begin() + N);
        tc.B.assign(vals.begin() + N, vals.end());
        tests.push_back(std::move(tc));
        /* === Variable Section End === */
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto &tc : tests) {
        /* === Variable Section === */
        for (int i = 0; i < N; ++i) o_a.write(tc.A[i]);
        for (int i = 0; i < N; ++i) o_b.write(tc.B[i]);
        /* === Variable Section End === */
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
        /* === Variable Section === */
        std::vector<int> vals;
        int v;
        while (iss >> v) vals.push_back(v);
        if (vals.size() != 2 * N) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << '\n';
            continue;
        }
        Testcase tc;
        tc.A.assign(vals.begin(), vals.begin() + N);
        tc.B.assign(vals.begin() + N, vals.end());
        tests.push_back(std::move(tc));
        /* === Variable Section End === */
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
        /* === Variable Section === */
        std::vector<int> vals;
        int v;
        while (iss >> v) vals.push_back(v);
        if (vals.size() != N) {
            std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << '\n';
            continue;
        }
        goldens.push_back({std::move(vals)});
        /* === Variable Section End === */
    }
    gin.close();

    wait(1);

    size_t passed_count = 0;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        std::vector<int> result;
        result.reserve(N);
        for (int i = 0; i < N; ++i) {
            int x = i_c.read();
            result.push_back(x);
        }

        bool passed = (result == goldens[idx].C);
        if (passed) ++passed_count;
    }
    /* === Variable Section End === */

    if (passed_count == goldens.size()) {
        std::cout << "All test cases passed!\n";
    } else {
        std::cerr << goldens.size() - passed_count << " out of " << goldens.size() << " test cases failed.\n";
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


def _build_prompt(
    func_code: str = "",
    dut_cpp: str = "",
    dut_h: str = "",
    requirement: str = "",
    system_prompt: str = _SYSTEM_PROMPT,
    response_h: str = None,
) -> str:
    if "qwen" in MODEL_NAME.lower():
        system_prompt = prompt._QWEN_SYSTEM_PROMPT_HEAD + system_prompt

    if func_code:
        ...  # 下面註解的地方好像用不到，我沒改
        # messages = [
        #     {"role": "system", "content": system_prompt},
        #     {
        #         "role": "user",
        #         "content": f"[Requirement]\n{_EXAMPLE_REQUIREMENT_1}\n```cpp\n{_EXAMPLE_FUNC_1}\n```",
        #     },
        #     {
        #         "role": "assistant",
        #         "content": _OUTPUT_FORMAT.format(
        #             Testbench_cpp=_EXAMPLE_TESTBENCH_CPP_1,
        #             Testbench_h=_EXAMPLE_TESTBENCH_H_1,
        #         ),
        #     },
        #     {
        #         "role": "user",
        #         "content": (
        #             (f"[Requirement]\n{requirement}" if requirement else "")
        #             + "\n```cpp\n"
        #             + func_code.strip()
        #             + "\n```"
        #         ),
        #     },
        # ]
    elif dut_cpp and dut_h:
        if requirement:
            requirement = re.sub(
                r"^\s*Given\b.*?equivalent\.\s*",
                "",
                requirement,
                # flags=re.DOTALL
            )
            requirement = _DESCRIPTION_WITH_DUT + f"\n{requirement.strip()}"
        else:
            requirement = _DESCRIPTION_WITH_DUT
        
        
        # [新增] 動態偵測浮點數並注入 Prompt
        # ============================================================
        # 檢查 Header 檔中是否定義了 float 或 double 類型的 Ports
        if "float" in dut_h or "double" in dut_h:
            requirement += (
                "\n\n[VERIFICATION CONSTRAINT]\n"
                "The DUT involves floating-point types. When writing the verification logic in do_fetch:\n"
                "1. Do NOT use exact equality (==).\n"
                "2. Use a tolerance check: if (std::abs(result - expected) > 1e-2) ...\n"
                "3. Ensure <cmath> is included."
            )
        # ============================================================  
        requirement += (
            "\n\n[FILE I/O CONSTRAINT - STRICT]\n"
            "1. The external files 'testcases.txt' and 'golden.txt' are PLAIN TEXT files containing numbers separated by spaces/newlines.\n"
            "2. FORBIDDEN: Do NOT use `std::ios::binary`.\n"
            "3. FORBIDDEN: Do NOT use `fin.read(...)` or `gin.read(...)` for block reading.\n"
            "4. REQUIRED: Use `std::ifstream` in default text mode and use the `>>` operator to read values one by one.\n"
            "5. If input/output is `unsigned char`, read it into an `int` variable first using `>>`, then static_cast.\n"
            "6. **MATRIX/ARRAY READING PATTERN (CRITICAL)**:\n"
            "   When reading a fixed-size structure (e.g., matrix[N][N]) inside the while loop:\n"
            "   - **Do NOT** simply verify `fin >> val` inside the loop condition without handling the data.\n"
            "   - **Pattern**: Use the first element to check stream validity, then read the rest.\n"
            "   ```cpp\n"
            "   int first_val;\n"
            "   while (fin >> first_val) { // 1. Read first element to check EOF\n"
            "       Testcase tc;\n"
            "       tc.matrix[0][0] = first_val; // 2. Assign first element\n"
            "       // 3. Read the REMAINING elements\n"
            "       for (int i = 0; i < SIZE; ++i) {\n"
            "           for (int j = 0; j < SIZE; ++j) {\n"
            "               if (i == 0 && j == 0) continue; // Skip the one already read\n"
            "               if (!(fin >> tc.matrix[i][j])) {\n"
            "                   std::cerr << \"Error: Premature EOF\\n\"; sc_stop(); return;\n"
            "               }\n"
            "           }\n"
            "       }\n"
            "       tests.push_back(tc);\n"
            "   }\n"
            "   ```"
        )

        messages = [
            {"role": "system", "content": system_prompt},
            # Few Shot - 1
            {
                "role": "user",
                "content": _QUERY_FORMAT.format(
                    REQUIREMENT_WITH_DUT=_EXAMPLE_REQUIREMENT_WITH_DUT_1,
                    DUT_H=_EXAMPLE_DUT_H_1,
                    DUT_CPP=_EXAMPLE_DUT_CPP_1,
                    GENERATE=_GENERATE_H,
                ),
            },
            {
                "role": "assistant",
                "content": _CODE_FORMAT.format(CODE=_EXAMPLE_TESTBENCH_H_1),
            },
            {
                "role": "user",
                "content": _GENERATE_CPP,
            },
            {
                "role": "assistant",
                "content": _CODE_FORMAT.format(CODE=_EXAMPLE_TESTBENCH_CPP_1),
            },
            # Few Shot - 2
            {
                "role": "user",
                "content": _QUERY_FORMAT.format(
                    REQUIREMENT_WITH_DUT=_EXAMPLE_REQUIREMENT_WITH_DUT_2,
                    DUT_H=_EXAMPLE_DUT_H_2,
                    DUT_CPP=_EXAMPLE_DUT_CPP_2,
                    GENERATE=_GENERATE_H,
                ),
            },
            {
                "role": "assistant",
                "content": _CODE_FORMAT.format(CODE=_EXAMPLE_TESTBENCH_H_2),
            },
            {
                "role": "user",
                "content": _GENERATE_CPP,
            },
            {
                "role": "assistant",
                "content": _CODE_FORMAT.format(CODE=_EXAMPLE_TESTBENCH_CPP_2),
            },
            # REAL QUERY
            {
                "role": "user",
                "content": _QUERY_FORMAT.format(
                    REQUIREMENT_WITH_DUT=requirement,
                    DUT_H=dut_h.strip(),
                    DUT_CPP=dut_cpp.strip(),
                    GENERATE=_GENERATE_H,
                ),
            },
        ]
    else:
        raise ValueError("Either func_code or both dut_cpp and dut_h must be provided.")

    if response_h is not None:
        messages.append({"role": "assistant", "content": response_h})
        messages.append({"role": "user", "content": _GENERATE_CPP})

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
    file_map = {"Testbench.h": code_h, "Testbench.cpp": code_cpp}
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


# ---------------------------------------------------------------------------
# Generate Testbench.cpp, Testbench.h via one-shot in-context learning
# ---------------------------------------------------------------------------
def generate_tb(
    func_code: str = "",
    dut_cpp: str = "",
    dut_h: str = "",
    requirement: str = "",
    system_prompt: str = _SYSTEM_PROMPT,
) -> dict[str, str]:
    ...  # 這部分好像不會用到，我沒改
    # messages = _build_prompt(
    #     func_code, dut_cpp, dut_h, requirement, system_prompt=system_prompt
    # )
    # raw = _llm.generate(messages).strip()

    # return _parse_tb_output(raw)


def generate_tb_batch(
    func_codes: List[str] | None = None,
    dut_cpp: List[str] | None = None,
    dut_h: List[str] | None = None,
    requirement: List[str] | None = None,
    system_prompt: str = _SYSTEM_PROMPT,
    *,
    max_new_tokens: int = 4096,
    temperature: float = 0.3,
    top_p: float = 0.8,
) -> List[Dict[str, str]]:

    if dut_cpp is not None and dut_h is not None:
        # --- DUT mode (takes precedence even if func_codes is also given) ---
        n = len(dut_cpp)
        if len(dut_h) != n:
            raise ValueError("dut_cpp and dut_h must have the same length")
        if func_codes is None:
            func_codes = [""] * n  # placeholder, not used in DUT mode
        elif len(func_codes) != n:
            raise ValueError("func_codes length must match dut_cpp/dut_h length")
    else:
        # --- Function‑call mode ---
        if func_codes is None:
            raise ValueError(
                "Either func_codes must be provided, or both dut_cpp and dut_h must be provided."
            )
        n = len(func_codes)
        dut_cpp = [""] * n  # placeholders for prompt builder
        dut_h = [""] * n

    # -------- requirement is optional --------
    if requirement is None:
        requirement = [""] * n
    elif len(requirement) != n:
        raise ValueError("requirement length must match batch size")

    print("[TB] Generating Testbench.h")
    ret = [
        _build_prompt(code, dut_cpp, dut_h, req, system_prompt=system_prompt)
        for code, dut_cpp, dut_h, req in zip(func_codes, dut_cpp, dut_h, requirement)
    ]
    prompts = [r["prompt"] for r in ret]
    responses_h = _llm.generate_batch(
        prompts,
        temperature=temperature,
        top_p=top_p,
        max_new_tokens=max_new_tokens,
    )

    print("[TB] Generating Testbench.cpp")
    ret = [
        _build_prompt(code, dut_cpp, dut_h, req, system_prompt, resp_h)
        for code, dut_cpp, dut_h, req, resp_h in zip(
            func_codes, dut_cpp, dut_h, requirement, responses_h
        )
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
            print(f"[generate_tb_batch] parse error: {e}")
            results.append({"Testbench.h": "", "Testbench.cpp": ""})
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
        globals().get("_parse_output"),
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
