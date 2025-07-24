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
    std::vector<float> input;
    std::ifstream fin("input.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open input.txt\n";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        float value;
        while (iss >> value) {
            input.push_back(value);
        }
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < input.size(); ++idx) {
        o_input.write(input[idx]);
        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<float> golden;
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
        float value;
        while (iss >> value) {
            golden.push_back(value);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < golden.size(); ++idx) {
        float result = i_output.read();
        bool passed = (result == golden[idx]);
        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Output: " << result << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Output: " << result << ", Expected: " << golden[idx] << "\n\n";
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
