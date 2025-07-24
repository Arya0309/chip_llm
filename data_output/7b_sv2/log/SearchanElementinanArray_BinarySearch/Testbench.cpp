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
        std::vector<int> arr;
        int target;
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
        int num;
        while (iss >> num) {
            tc.arr.push_back(num);
        }
        if (tc.arr.size() != 7) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            continue;
        }
        tc.target = tc.arr.back();
        tc.arr.pop_back();
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
        const std::vector<int>& arr = tests[idx].arr;
        int target = tests[idx].target;
        /* === Variable Section End === */

        /* === Variable Section === */
        for (int num : arr) {
            o_arr.write(num);
        }
        o_target.write(target);
        /* === Variable Section End === */

        wait();
    }
}

void Testbench::do_fetch() {
    struct Testcase {
        std::vector<int> arr;
        int target;
    };
    struct Golden {
        bool expected;
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
        int num;
        while (iss >> num) {
            tc.arr.push_back(num);
        }
        if (tc.arr.size() != 7) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
            continue;
        }
        tc.target = tc.arr.back();
        tc.arr.pop_back();
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<Golden> goldens;
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
            Golden g;
            if (!(iss >> g.expected)) {
                std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << "\n";
                continue;
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        bool result;

        result = i_result.read();

        bool passed = (result == goldens[idx].expected);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: arr = [";
            for (size_t i = 0; i < tests[idx].arr.size(); ++i) {
                std::cout << tests[idx].arr[i];
                if (i != tests[idx].arr.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "], target = " << tests[idx].target << "\n";
            std::cout << "Output: " << result << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: arr = [";
            for (size_t i = 0; i < tests[idx].arr.size(); ++i) {
                std::cout << tests[idx].arr[i];
                if (i != tests[idx].arr.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "], target = " << tests[idx].target << "\n";
            std::cerr << "Output: " << result << ", Expected: " << goldens[idx].expected << "\n\n";
            all_passed = false;
        }
    }
    /* === Variable Section End === */

    if (all_passed) {
        std::cout << "All tests passed successfully.\" << std::endl;
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
