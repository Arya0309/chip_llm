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
    struct TestCase { std::vector<int> input; };
    std::vector<TestCase> tests;
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
        TestCase tc;
        int value;
        while (iss >> value) {
            tc.input.push_back(value);
        }
        tests.push_back(tc);
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto& test : tests) {
        for (int value : test.input) {
            o_input.write(value);
        }
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase { std::vector<int> input; };
    struct Golden { std::vector<int> expected; };

    std::vector<TestCase> tests;
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
        TestCase tc;
        int value;
        while (iss >> value) {
            tc.input.push_back(value);
        }
        tests.push_back(tc);
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
            int value;
            while (iss >> value) {
                g.expected.push_back(value);
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        std::vector<int> result;
        for (size_t i = 0; i < goldens[idx].expected.size(); ++i) {
            result.push_back(i_output.read());
        }

        bool passed = true;
        if (result.size() != goldens[idx].expected.size()) {
            passed = false;
        } else {
            for (size_t i = 0; i < result.size(); ++i) {
                if (result[i] != goldens[idx].expected[i]) {
                    passed = false;
                    break;
                }
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: ";
            for (int value : tests[idx].input) {
                std::cout << value << " ";
            }
            std::cout << "\nOutput: ";
            for (int value : result) {
                std::cout << value << " ";
            }
            std::cout << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: ";
            for (int value : tests[idx].input) {
                std::cerr << value << " ";
            }
            std::cerr << "\nOutput: ";
            for (int value : result) {
                std::cerr << value << " ";
            }
            std::cerr << "\nExpected: ";
            for (int value : goldens[idx].expected) {
                std::cerr << value << " ";
            }
            std::cerr << "\n\n";
            all_passed = false;
        }
    }

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
