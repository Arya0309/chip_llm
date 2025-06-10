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
    std::array<int, 7> in{};
    int                d{};
    std::array<int, 7> expected{};
    std::array<int, 7> out{};
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
        for (char& c : line)
            if (c == '|') c = ' ';
        std::istringstream iss(line);
        Testcase tc;
        for (int& v : tc.in)    iss >> v;
        iss >> tc.d;
        for (int& v : tc.expected) iss >> v;
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
    o_d.reset();
#endif

    const std::vector<Testcase> tests = load_testcases();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        const auto& tc = tests[idx];

        for (int i = 0; i < 7; ++i) {
    #ifndef NATIVE_SYSTEMC
            o_in_array.put(tc.in[i]);
    #else
            o_in_array.write(tc.in[i]);
    #endif
        }
    #ifndef NATIVE_SYSTEMC
        o_d.put(tc.d);
    #else
        o_d.write(tc.d);
    #endif
        wait();
    }
}

/* ─────────────────────────── FETCH ─────────────────────────── */
void Testbench::do_fetch() {
#ifndef NATIVE_SYSTEMC
    i_out_array.reset();
#endif

    /* 同樣把 testcases 讀成向量，用來比對 */
    std::vector<Testcase> tests = load_testcases();

    wait(2);   // 稍微等候，確保第一筆資料已經在路上

    bool global_flag = true;
    for (size_t idx = 0; idx < tests.size(); ++idx) {
        const auto& tc_read = tests[idx];
        auto& tc_write = tests[idx];

        std::cout << "Unit test " << idx + 1 << "\n";
        std::cout << "Input: "<< "arr = [";
        for (int i = 0; i < 7; ++i) {
            std::cout << tc_read.in[i] << (i == 6 ? "], " : ",");
        }
        std::cout << "rotation = " << tc_read.d << '\n';

        std::cout << "Output: [";
        for (int i = 0; i < 7; ++i) {
        #ifndef NATIVE_SYSTEMC
            int out_v = i_out_array.get();
        #else
            int out_v = i_out_array.read();
        #endif
            tc_write.out[i] = out_v;
            std::cout << out_v << (i == 6 ? "]\n" : ",");
            wait();
        }

        // 驗證輸出結果
        bool flag = true;
        for (int i = 0; i < 7; ++i) {
            if (tc_write.out[i] != tc_write.expected[i]) {
                flag = false;
                if (global_flag) {
                    global_flag = false;
                }
                break;
            }
        }
        if (flag) {
            std::cout <<  "Test case passed.\n\n";
        } else {
            std::cout << "Assertion failed, "
                      << "expected output: [";
            for (int i = 0; i < 7; ++i) {
                std::cout << tc_write.expected[i] << (i == 6 ? "]\n\n" : ",");
            }
            
        }
    }
    if (global_flag) {
        std::cout << "All tests passed successfully." << std::endl;
    }
    sc_stop();
}
