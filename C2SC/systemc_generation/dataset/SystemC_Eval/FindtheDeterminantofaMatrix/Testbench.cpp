// Updated Testbench for MatrixMultiply, using external file and detailed output
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Testbench.h"

struct Testcase {
    std::vector<int> A;
    std::vector<int> expected;
};

Testbench::Testbench(sc_module_name n)
    : sc_module(n) {
    SC_THREAD(do_feed);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(do_fetch);
    sensitive << i_clk.pos();
    dont_initialize();
}

void Testbench::do_feed() {
    // Read testcases from file
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
        int val;
        while (iss >> val) tokens.push_back(val);
        const size_t expected_size = A_ROWS*A_COLS + 1; // A + expected
        if (tokens.size() != expected_size) {
            std::cerr << "Warning: Skipping line due to incorrect token count: " << line << "\n";
            continue;
        }
        Testcase tc;
        tc.A.assign(tokens.begin(), tokens.begin() + A_ROWS*A_COLS);
        tc.expected.assign(tokens.begin() + A_ROWS*A_COLS, tokens.end());
        tests.push_back(tc);
    }
    fin.close();

#ifndef NATIVE_SYSTEMC
    o_A.reset();
#endif

    // Apply reset
    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    // Feed each testcase
    for (size_t idx = 0; idx < tests.size(); ++idx) {
        const Testcase &tc = tests[idx];
        // Write Matrix A
        for (int i = 0; i < A_ROWS * A_COLS; ++i) {
#ifndef NATIVE_SYSTEMC
            o_A.put(tc.A[i]);
#else
            o_A.write(tc.A[i]);
#endif
        }
        // Wait one clock to latch inputs
        wait();
    }
}

void Testbench::do_fetch() {
#ifndef NATIVE_SYSTEMC
    i_C.reset();
#endif
    // Allow for reset and first feed
    wait(5);
    wait(1);

    // Read testcases again for verification
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
        int val;
        while (iss >> val) tokens.push_back(val);
        const size_t expected_size = A_ROWS*A_COLS + 1; // A + expected
        if (tokens.size() != expected_size) continue;
        Testcase tc;
        tc.A.assign(tokens.begin(), tokens.begin() + A_ROWS*A_COLS);
        tc.expected.assign(tokens.begin() + A_ROWS*A_COLS, tokens.end());
        tests.push_back(tc);
    }
    fin.close();

    bool global_flag = true;
    // Fetch and verify each testcase
    for (size_t idx = 0; idx < tests.size(); ++idx) {
        const Testcase &tc = tests[idx];
        std::vector<int> results;
        bool passed = true;
#ifndef NATIVE_SYSTEMC
        int res = i_C.get();
#else
        int res = i_C.read();
#endif
        results.push_back(res);

        if (res != tc.expected[0]){ 
            passed = false;
            global_flag = false;
        }

        if (passed) {
            std::cout << "Test case " << idx+1 << " passed." << std::endl;
            std::cout << "Input Matrix:" << std::endl;
            for (int r = 0; r < A_ROWS; ++r) {
                for (int c = 0; c < A_COLS; ++c)
                    std::cout << tc.A[r*A_COLS + c] << " ";
                std::cout << std::endl;
            }

            std::cout << "Output: " << results[0] << "\n" << std::endl;
        } else {
            std::cerr << "Test case " << idx+1 << " failed." << std::endl;
            std::cerr << "Input Matrix:" << std::endl;
            for (int r = 0; r < A_ROWS; ++r) {
                for (int c = 0; c < A_COLS; ++c)
                    std::cerr << tc.A[r*A_COLS + c] << " ";
                std::cerr << std::endl;
            }
            std::cerr << "Output: " << results[0] << std::endl;
            std::cerr << "Expected: " << tc.expected[0] << "\n" << std::endl;
        }
    }

    if (global_flag) {
        std::cout << "All tests passed successfully." << std::endl;
    } else {
        SC_REPORT_FATAL("Testbench", "Assertion failed");
    }
    sc_stop();
}
