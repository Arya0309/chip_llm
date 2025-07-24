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
        std::vector<double> input;
        std::vector<double> expected_output;
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
        double value;
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

    for (const auto& tc : tests) {
        /* === Variable Section === */
        for (double val : tc.input) {
            o_a.write(val);
        }
        /* === Variable Section End === */

        wait();
    }
}

void Testbench::do_fetch() {
    struct Testcase {
        std::vector<double> input;
        std::vector<double> expected_output;
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
        double value;
        while (iss >> value) {
            tc.input.push_back(value);
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<std::vector<double>> goldens;
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
            std::vector<double> golden;
            double value;
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
    for (size_t idx = 0; idx < tests.size(); ++idx) {
        std::vector<double> output;

        for (size_t i = 0; i < tests[idx].input.size(); ++i) {
            output.push_back(i_result.read());
        }

        bool passed = true;
        for (size_t i = 0; i < output.size(); ++i) {
            if (fabs(output[i] - goldens[idx][i]) > 1e-6) {
                passed = false;
                break;
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed." << std::endl;
            std::cout << "Input: [";
            for (size_t i = 0; i < tests[idx].input.size(); ++i) {
                std::cout << tests[idx].input[i] << " ";
            }
            std::cout << "]" << std::endl;
            std::cout << "Output: [";
            for (size_t i = 0; i < output.size(); ++i) {
                std::cout << output[i] << " ";
            }
            std::cout << "]" << std::endl;
        } else {
            std::cerr << "Test case " << idx + 1 << " failed." << std::endl;
            std::cerr << "Input: [";
            for (size_t i = 0; i < tests[idx].input.size(); ++i) {
                std::cout << tests[idx].input[i] << " ";
            }
            std::cout << "]" << std::endl;
            std::cerr << "Output: [";
            for (size_t i = 0; i < output.size(); ++i) {
                std::cout << output[i] << " ";
            }
            std::cout << "]" << std::endl;
            std::cerr << "Expected: [";
            for (size_t i = 0; i < goldens[idx].size(); ++i) {
                std::cout << goldens[idx][i] << " ";
            }
            std::cout << "]" << std::endl;
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
