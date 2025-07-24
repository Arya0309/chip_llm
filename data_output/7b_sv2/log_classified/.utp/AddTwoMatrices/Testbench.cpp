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
    constexpr int N = 4;
    struct Testcase {
        std::array<int, N * N> a;
        std::array<int, N * N> b;
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
        for (int i = 0; i < N * N; ++i) {
            if (!(iss >> tc.a[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
                continue;
            }
        }
        for (int i = 0; i < N * N; ++i) {
            if (!(iss >> tc.b[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
                continue;
            }
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
        const auto& tc = tests[idx];
        /* === Variable Section End === */

        /* === Variable Section === */
        for (int i = 0; i < N * N; ++i) {
            o_a.write(tc.a[i]);
        }
        for (int i = 0; i < N * N; ++i) {
            o_b.write(tc.b[i]);
        }
        /* === Variable Section End === */

        wait();
    }
}

void Testbench::do_fetch() {
    constexpr int N = 4;
    struct Testcase {
        std::array<int, N * N> a;
        std::array<int, N * N> b;
    };
    struct Golden {
        std::array<int, N * N> expected;
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
        for (int i = 0; i < N * N; ++i) {
            if (!(iss >> tc.a[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << std::endl;
                continue;
            }
        }
        for (int i = 0; i < N * N; ++i) {
            if (!(iss >> tc.b[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << std::endl;
                continue;
            }
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<Golden> goldens;
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
            Golden g;
            for (int i = 0; i < N * N; ++i) {
                if (!(iss >> g.expected[i])) {
                    std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << std::endl;
                    continue;
                }
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        std::array<int, N * N> result;

        for (int i = 0; i < N * N; ++i) {
            result[i] = i_result.read();
        }

        bool passed = true;
        for (int i = 0; i < N * N; ++i) {
            if (result[i] != goldens[idx].expected[i]) {
                passed = false;
                break;
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed." << std::endl;
            std::cout << "Input: a = [";
            for (int i = 0; i < N * N; ++i) {
                std::cout << tests[idx].a[i] << " ";
            }
            std::cout << "]" << std::endl;
            std::cout << "Input: b = [";
            for (int i = 0; i < N * N; ++i) {
                std::cout << tests[idx].b[i] << " ";
            }
            std::cout << "]" << std::endl;
            std::cout << "Output: [";
            for (int i = 0; i < N * N; ++i) {
                std::cout << result[i] << " ";
            }
            std::cout << "]" << std::endl;
        } else {
            std::cerr << "Test case " << idx + 1 << " failed." << std::endl;
            std::cerr << "Input: a = [";
            for (int i = 0; i < N * N; ++i) {
                std::cout << tests[idx].a[i] << " ";
            }
            std::cout << "]" << std::endl;
            std::cerr << "Input: b = [";
            for (int i = 0; i < N * N; ++i) {
                std::cout << tests[idx].b[i] << " ";
            }
            std::cout << "]" << std::endl;
            std::cerr << "Output: [";
            for (int i = 0; i < N * N; ++i) {
                std::cout << result[i] << " ";
            }
            std::cerr << "]" << std::endl;
            std::cerr << "Expected: [";
            for (int i = 0; i < N * N; ++i) {
                std::cerr << goldens[idx].expected[i] << " ";
            }
            std::cerr << "]" << std::endl;
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
