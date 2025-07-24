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
        std::vector<int> input;
        std::vector<int> expected_output;
    };
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
        int value;
        while (iss >> value) {
            tc.input.push_back(value);
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
        const auto& input = tests[idx].input;
        /* === Variable Section End === */

        /* === Variable Section === */
        for (int val : input) {
            o_a.write(val);
        }
        /* === Variable Section End === */

        wait();
    }
}

void Testbench::do_fetch() {
    struct Testcase {
        std::vector<int> input;
        std::vector<int> expected_output;
    };
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
        int value;
        while (iss >> value) {
            tc.input.push_back(value);
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<std::vector<int>> goldens;
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
            std::vector<int> golden;
            int value;
            while (iss >> value) {
                golden.push_back(value);
            }
            goldens.push_back(golden);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        std::vector<int> result;

        for (int i = 0; i < 6; ++i) {
            result.push_back(i_result.read());
        }

        bool passed = (result == goldens[idx]);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: [";
            for (size_t i = 0; i < tests[idx].input.size(); ++i) {
                std::cout << tests[idx].input[i];
                if (i != tests[idx].input.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "]\n";
            std::cout << "Output: [";
            for (size_t i = 0; i < result.size(); ++i) {
                std::cout << result[i];
                if (i != result.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "]\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: [";
            for (size_t i = 0; i < tests[idx].input.size(); ++i) {
                std::cerr << tests[idx].input[i];
                if (i != tests[idx].input.size() - 1) {
                    std::cerr << ", ";
                }
            }
            std::cerr << "]\n";
            std::cerr << "Output: [";
            for (size_t i = 0; i < result.size(); ++i) {
                std::cerr << result[i];
                if (i != result.size() - 1) {
                    std::cerr << ", ";
                }
            }
            std::cerr << "]\nExpected: [";
            for (size_t i = 0; i < goldens[idx].size(); ++i) {
                std::cerr << goldens[idx][i];
                if (i != goldens[idx].size() - 1) {
                    std::cerr << ", ";
                }
            }
            std::cerr << "]\n\n";
            all_passed = false;
        }
    }
    /* === Variable Section End === */

    if (all_passed) {
        std::cout << "All tests passed successfully.\n";
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
