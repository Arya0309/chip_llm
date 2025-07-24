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
    struct Testcase {
        double X[N * D];
        int y[N];
        int n;
        int d;
        double C;
        double lr;
        int epochs;
        double w[D];
        double b;
    };

    std::vector<Testcase> tests;
    std::ifstream fin("testcases.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt" << std::endl;
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        Testcase tc;
        for (int i = 0; i < N * D; ++i) {
            iss >> tc.X[i];
        }
        for (int i = 0; i < N; ++i) {
            iss >> tc.y[i];
        }
        iss >> tc.n >> tc.d >> tc.C >> tc.lr >> tc.epochs;
        for (int i = 0; i < D; ++i) {
            iss >> tc.w[i];
        }
        iss >> tc.b;
        /* === Variable Section End === */
        tests.push_back(tc);
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        /* === Variable Section === */
        const Testcase& tc = tests[idx];
        /* === Variable Section End === */

        /* === Variable Section === */
        for (int i = 0; i < N * D; ++i) {
            o_X.write(tc.X[i]);
        }
        for (int i = 0; i < N; ++i) {
            o_y.write(tc.y[i]);
        }
        o_n.write(tc.n);
        o_d.write(tc.d);
        o_C.write(tc.C);
        o_lr.write(tc.lr);
        o_epochs.write(tc.epochs);
        for (int i = 0; i < D; ++i) {
            o_w.write(tc.w[i]);
        }
        o_b.write(tc.b);
        /* === Variable Section End === */

        wait();
    }
}

void Testbench::do_fetch() {
    struct Testcase {
        double X[N * D];
        int y[N];
        int n;
        int d;
        double C;
        double lr;
        int epochs;
        double w[D];
        double b;
    };

    std::vector<Testcase> tests;
    std::ifstream fin("testcases.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt" << std::endl;
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        Testcase tc;
        for (int i = 0; i < N * D; ++i) {
            iss >> tc.X[i];
        }
        for (int i = 0; i < N; ++i) {
            iss >> tc.y[i];
        }
        iss >> tc.n >> tc.d >> tc.C >> tc.lr >> tc.epochs;
        for (int i = 0; i < D; ++i) {
            iss >> tc.w[i];
        }
        iss >> tc.b;
        /* === Variable Section End === */
        tests.push_back(tc);
    }
    fin.close();

    std::vector<double> goldens;
    {
        std::ifstream fin("golden.txt");
        if (!fin.is_open()) {
            std::cerr << "Error: Unable to open golden.txt" << std::endl;
            sc_stop();
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            double golden;
            if (!(iss >> golden)) {
                std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << std::endl;
                continue;
            }
            goldens.push_back(golden);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        double result;

        result = i_w.read();

        bool passed = (result == goldens[idx]);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed." << std::endl;
            std::cout << "Input: n = " << tests[idx].n << ", d = " << tests[idx].d << ", C = " << tests[idx].C << ", lr = " << tests[idx].lr << ", epochs = " << tests[idx].epochs << std::endl;
            std::cout << "Output: " << result << std::endl;
        } else {
            std::cerr << "Test case " << idx + 1 << " failed." << std::endl;
            std::cerr << "Input: n = " << tests[idx].n << ", d = " << tests[idx].d << ", C = " << tests[idx].C << ", lr = " << tests[idx].lr << ", epochs = " << tests[idx].epochs << std::endl;
            std::cerr << "Output: " << result << ", Expected: " << goldens[idx] << std::endl;
            all_passed = false;
        }
    }
    /* === Variable Section End === */

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
