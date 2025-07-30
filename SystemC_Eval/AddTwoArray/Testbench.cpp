// Testbench.cpp — AddTwoArray
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

        if (passed) {
            ++passed_count;
            std::cout << "Test case " << idx + 1 << " passed.\n";
        } else {
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
            for (int x : result) std::cerr << x << " ";
            std::cerr << "\nExpected: ";
            for (int x : goldens[idx].C) std::cerr << x << " ";
            std::cerr << "\n";
        }
    }
    /* === Variable Section End === */

    if (passed_count != goldens.size()) {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
