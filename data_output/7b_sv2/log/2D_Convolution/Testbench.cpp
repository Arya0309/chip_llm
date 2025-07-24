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
    std::vector<std::vector<double>> inputs;
    std::vector<std::vector<double>> kernels;
    std::vector<std::vector<double>> expected_outputs;

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
        std::vector<double> input, kernel, expected_output;

        int h, w;
        iss >> h >> w;

        for (int i = 0; i < h * w; ++i) {
            double val;
            iss >> val;
            input.push_back(val);
        }

        iss >> h >> w;

        for (int i = 0; i < h * w; ++i) {
            double val;
            iss >> val;
            kernel.push_back(val);
        }

        iss >> h >> w;

        for (int i = 0; i < h * w; ++i) {
            double val;
            iss >> val;
            expected_output.push_back(val);
        }

        inputs.push_back(input);
        kernels.push_back(kernel);
        expected_outputs.push_back(expected_output);
    }

    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < inputs.size(); ++idx) {
        int in_h = std::sqrt(inputs[idx].size());
        int in_w = in_h;
        int ker_h = std::sqrt(kernels[idx].size());
        int ker_w = ker_h;

        for (const auto& val : inputs[idx]) {
            o_input.write(val);
        }

        for (const auto& val : kernels[idx]) {
            o_kernel.write(val);
        }

        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<std::vector<double>> inputs;
    std::vector<std::vector<double>> kernels;
    std::vector<std::vector<double>> expected_outputs;

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
        std::vector<double> input, kernel, expected_output;

        int h, w;
        iss >> h >> w;

        for (int i = 0; i < h * w; ++i) {
            double val;
            iss >> val;
            input.push_back(val);
        }

        iss >> h >> w;

        for (int i = 0; i < h * w; ++i) {
            double val;
            iss >> val;
            kernel.push_back(val);
        }

        iss >> h >> w;

        for (int i = 0; i < h * w; ++i) {
            double val;
            iss >> val;
            expected_output.push_back(val);
        }

        inputs.push_back(input);
        kernels.push_back(kernel);
        expected_outputs.push_back(expected_output);
    }

    fin.close();

    wait(1);

    bool all_passed = true;

    for (size_t idx = 0; idx < expected_outputs.size(); ++idx) {
        int in_h = std::sqrt(inputs[idx].size());
        int in_w = in_h;
        int ker_h = std::sqrt(kernels[idx].size());
        int ker_w = ker_h;

        double output[in_h * in_w];

        for (const auto& val : inputs[idx]) {
            o_input.write(val);
        }

        for (const auto& val : kernels[idx]) {
            o_kernel.write(val);
        }

        wait(in_h * in_w);

        for (int i = 0; i < in_h * in_w; ++i) {
            output[i] = i_output.read();
        }

        bool passed = true;

        for (size_t i = 0; i < expected_outputs[idx].size(); ++i) {
            if (output[i] != expected_outputs[idx][i]) {
                passed = false;
                break;
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            all_passed = false;
        }
    }

    if (all_passed) {
        std::cout << "All tests passed successfully.\n";
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }

    sc_stop();
}
