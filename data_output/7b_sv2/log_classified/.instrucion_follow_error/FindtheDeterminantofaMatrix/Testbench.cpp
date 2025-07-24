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
        double a[N][N];
        double expected;
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
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (!(iss >> tc.a[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
                    continue;
                }
            }
        }
        if (!(iss >> tc.expected)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
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
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                o_a.write(tests[idx].a[i][j]);
            }
        }
        /* === Variable Section End === */

        wait();
    }
}

void Testbench::do_fetch() {
    struct Testcase {
        double a[N][N];
        double expected;
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
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (!(iss >> tc.a[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << std::endl;
                    continue;
                }
            }
        }
        if (!(iss >> tc.expected)) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << std::endl;
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < tests.size(); ++idx) {
        double result;

        result = i_result.read();

        bool passed = (std::abs(result - tests[idx].expected) < 1e-6);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed." << std::endl;
            std::cout << "Input: \n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cout << tests[idx].a[i][j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "Output: " << result << std::endl;
        } else {
            std::cerr << "Test case " << idx + 1 << " failed." << std::endl;
            std::cerr << "Input: \n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cerr << tests[idx].a[i][j] << " ";
                }
                std::cerr << std::endl;
            }
            std::cerr << "Output: " << result << ", Expected: " << tests[idx].expected << std::endl;
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
