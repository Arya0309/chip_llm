#include "Testbench.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

constexpr int LENGTH = 5;

void softmax(const double* input, double* output, int length) {
    double max_val = input[0];
    for (int i = 1; i < length; ++i) {
        if (input[i] > max_val) {
            max_val = input[i];
        }
    }

    double sum = 0.0;
    for (int i = 0; i < length; ++i) {
        output[i] = exp(input[i] - max_val);
        sum += output[i];
    }

    for (int i = 0; i < length; ++i) {
        output[i] /= sum;
    }
}

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
    struct Testcase { double inputs[LENGTH]; };
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
        for (int i = 0; i < LENGTH; ++i) {
            if (!(iss >> tc.inputs[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                break;
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
        for (int i = 0; i < LENGTH; ++i) {
            o_input.write(tests[idx].inputs[i]);
        }
        /* === Variable Section End === */

        wait();
    }
}


void Testbench::do_fetch() {
    struct Testcase { double inputs[LENGTH]; };
    struct Golden { double outputs[LENGTH]; };

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
        for (int i = 0; i < LENGTH; ++i) {
            if (!(iss >> tc.inputs[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                break;
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
            std::cerr << "Error: Unable to open golden.txt\n";
            sc_stop();
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            Golden g;
            for (int i = 0; i < LENGTH; ++i) {
                if (!(iss >> g.outputs[i])) {
                    std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << "\n";
                    break;
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
        double results[LENGTH];
        for (int i = 0; i < LENGTH; ++i) {
            results[i] = i_output.read();
        }

        bool passed = true;
        for (int i = 0; i < LENGTH; ++i) {
            if (fabs(results[i] - goldens[idx].outputs[i]) > 1e-6) {
                passed = false;
                break;
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Inputs: ";
            for (int i = 0; i < LENGTH; ++i) {
                std::cout << tests[idx].inputs[i] << " ";
            }
            std::cout << "\nOutputs: ";
            for (int i = 0; i < LENGTH; ++i) {
                std::cout << results[i] << " ";
            }
            std::cout << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Inputs: ";
            for (int i = 0; i < LENGTH; ++i) {
                std::cerr << tests[idx].inputs[i] << " ";
            }
            std::cerr << "\nOutputs: ";
            for (int i = 0; i < LENGTH; ++i) {
                std::cerr << results[i] << " ";
            }
            std::cerr << "\nExpected: ";
            for (int i = 0; i < LENGTH; ++i) {
                std::cerr << goldens[idx].outputs[i] << " ";
            }
            std::cerr << "\n\n";
            all_passed = false;
        }
    }
    /* === Variable Section End === */

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    }
    else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
