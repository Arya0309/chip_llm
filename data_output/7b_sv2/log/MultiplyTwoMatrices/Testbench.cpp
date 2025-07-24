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
        std::vector<int> mat1;
        std::vector<int> mat2;
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
            tc.mat1.push_back(value);
        }
        std::getline(fin, line);
        iss.clear();
        iss.str(line);
        while (iss >> value) {
            tc.mat2.push_back(value);
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
        for (int val : tc.mat1) {
            o_mat1.write(val);
        }
        for (int val : tc.mat2) {
            o_mat2.write(val);
        }
        /* === Variable Section End === */

        wait();
    }
}

void Testbench::do_fetch() {
    struct Testcase {
        std::vector<int> mat1;
        std::vector<int> mat2;
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
            tc.mat1.push_back(value);
        }
        std::getline(fin, line);
        iss.clear();
        iss.str(line);
        while (iss >> value) {
            tc.mat2.push_back(value);
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<int> expected_results;
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
            int value;
            while (iss >> value) {
                expected_results.push_back(value);
            }
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < tests.size(); ++idx) {
        int result;

        result = i_result.read();

        bool passed = (result == expected_results[idx]);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: mat1 = [";
            for (size_t i = 0; i < tests[idx].mat1.size(); ++i) {
                std::cout << tests[idx].mat1[i] << " ";
            }
            std::cout << "]\nInput: mat2 = [";
            for (size_t i = 0; i < tests[idx].mat2.size(); ++i) {
                std::cout << tests[idx].mat2[i] << " ";
            }
            std::cout << "]\nOutput: " << result << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: mat1 = [";
            for (size_t i = 0; i < tests[idx].mat1.size(); ++i) {
                std::cout << tests[idx].mat1[i] << " ";
            }
            std::cout << "]\nInput: mat2 = [";
            for (size_t i = 0; i < tests[idx].mat2.size(); ++i) {
                std::cout << tests[idx].mat2[i] << " ";
            }
            std::cout << "]\nOutput: " << result << ", Expected: " << expected_results[idx] << "\n\n";
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
