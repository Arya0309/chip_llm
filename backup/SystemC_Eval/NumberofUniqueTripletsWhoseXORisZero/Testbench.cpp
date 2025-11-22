#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Testbench.h"

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

struct Testcase {
    std::array<int, 5> in{};
    int expected;
};

static std::vector<Testcase> load_testcases(const char* fname = "testcases.txt") {
    std::vector<Testcase> tests;
    std::ifstream fin(fname);
    if (!fin) {
        std::cerr << "Error: cannot open " << fname << '\n';
        return tests;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#')
            continue;
        std::istringstream iss(line);
        Testcase tc;
        for (int& v : tc.in)    iss >> v;
        iss >> tc.expected;
        if (iss.fail()) {
            std::cerr << "Warning: skip malformed line → " << line << '\n';
            continue;
        }
        tests.push_back(tc);
    }
    return tests;
}

/* ─────────────────────────── FEED ─────────────────────────── */
void Testbench::do_feed() {
#ifndef NATIVE_SYSTEMC
    o_in_array.reset();
#endif

    const auto tests = load_testcases();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto& tc : tests) {
        for (int i = 0; i < 5; ++i) {
#ifndef NATIVE_SYSTEMC
            o_in_array.put(tc.in[i]);
#else
            o_in_array.write(tc.in[i]);
#endif
        }
        wait();
    }
}

/* ─────────────────────────── FETCH ─────────────────────────── */
void Testbench::do_fetch() {
#ifndef NATIVE_SYSTEMC
    i_out.reset();
#endif

    const auto tests = load_testcases();
    wait(2);

    bool global_flag = true;
    for (size_t idx = 0; idx < tests.size(); ++idx) {
        const auto& tc = tests[idx];
#ifdef NATIVE_SYSTEMC
        int out = i_out.read();
#else
        int out = i_out.get();
#endif
        if (out == tc.expected) {
            std::cout << "Test case " << idx+1 << " passed. ";
            std::cout << "Input: arr = [";
            for (int i = 0; i < 5; ++i)
                std::cout << tc.in[i] << (i==4?"], ": ",");
            std::cout << "Output: " << out << '\n';
        } else {
            std::cerr << "Test case " << idx+1 << " failed. ";
            std::cerr << "Input: arr = [";
            for (int i = 0; i < 5; ++i)
                std::cerr << tc.in[i] << (i==4?"], ": ",");
            std::cerr << "Output: " << out
                      << " Expected: " << tc.expected << '\n';
            global_flag = false;
        }
    }
    if (global_flag) {
        std::cout << "All tests passed successfully.\n";
    } else {
        SC_REPORT_FATAL("Testbench", "Assertion failed");
    }
    sc_stop();
}
