#include <cassert>
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
    struct Testcase { int a, b, expected; };
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
        if (!(iss >> tc.a >> tc.b >> tc.expected)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

#ifndef NATIVE_SYSTEMC
    /* === Variable Section === */
    o_a.reset();
    o_b.reset();
    /* === Variable Section End === */
#endif

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
#ifndef NATIVE_SYSTEMC
        o_a.put(a);
        o_b.put(b);
#else
        o_a.write(a);
        o_b.write(b);
#endif
        wait();
    }
}
/* === Variable Section End === */

void Testbench::do_fetch() {
    std::vector<int> input_a;
    std::vector<int> input_b;
    std::vector<int> expecteds;
    std::ifstream fout("testcases.txt");
    if (!fout.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt\n";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fout, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        int a, b, e;
        if (!(iss >> a >> b >> e)) continue;
        input_a.push_back(a);
        input_b.push_back(b);
        expecteds.push_back(e);
        /* === Variable Section End === */
    }
    fout.close();

#ifndef NATIVE_SYSTEMC
    i_result.reset();
#endif

    wait(1);

    bool global_flag = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < expecteds.size(); ++idx) {
        int result;
#ifndef NATIVE_SYSTEMC
        result = i_result.get();
#else
        result = i_result.read();
#endif
        std::cout << "Unit test " << idx + 1 << "\n";
        std::cout << "Input: x = " << input_a[idx] << ", y = " << input_b[idx] << '\n';
        std::cout << "Output: " << result << '\n';

        bool flag = (result == expecteds[idx]);

        if (flag) {
            std::cout << "Test case passed.\n\n";
        } else {
            std::cout << "Assertion failed, "
                      << "expected output: " << expecteds[idx] << "\n\n";
            if (global_flag) {
                global_flag = false;
            }
        }
    }
    /* === Variable Section End === */

    if (global_flag) {
        std::cout << "All tests passed successfully." << std::endl;
    }
    sc_stop();
}
