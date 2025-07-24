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
    std::vector<std::pair<double, double>> inputs;
    std::ifstream fin("input.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open input.txt" << std::endl;
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        double real, imag;
        if (!(iss >> real >> imag)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
            continue;
        }
        inputs.push_back({real, imag});
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto& [real, imag] : inputs) {
        o_a_real.write(real);
        o_a_imag.write(imag);
        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<std::pair<double, double>> expected_outputs;
    std::ifstream fin("expected_output.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open expected_output.txt" << std::endl;
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        double real, imag;
        if (!(iss >> real >> imag)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
            continue;
        }
        expected_outputs.push_back({real, imag});
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < expected_outputs.size(); ++idx) {
        double result_real = i_result_real.read();
        double result_imag = i_result_imag.read();

        bool passed = (result_real == expected_outputs[idx].first && result_imag == expected_outputs[idx].second);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed." << std::endl;
            std::cout << "Input: real = " << inputs[idx].first << ", imag = " << inputs[idx].second << std::endl;
            std::cout << "Output: real = " << result_real << ", imag = " << result_imag << std::endl;
        } else {
            std::cerr << "Test case " << idx + 1 << " failed." << std::endl;
            std::cerr << "Input: real = " << inputs[idx].first << ", imag = " << inputs[idx].second << std::endl;
            std::cerr << "Output: real = " << result_real << ", imag = " << result_imag << ", Expected: real = " << expected_outputs[idx].first << ", imag = " << expected_outputs[idx].second << std::endl;
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
