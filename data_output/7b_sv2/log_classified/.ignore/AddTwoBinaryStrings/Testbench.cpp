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
    std::vector<std::string> inputs;
    std::ifstream fin("inputs.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open inputs.txt\n";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        inputs.push_back(line);
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto& input : inputs) {
        if (input.length() != 16) {
            std::cerr << "Warning: Input length is not 16, skipping: " << input << "\n";
            continue;
        }
        for (int i = 0; i < 8; ++i) {
            o_a.write(input[i]);
        }
        for (int i = 8; i < 16; ++i) {
            o_b.write(input[i]);
        }
        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<std::string> expected_outputs;
    std::ifstream fin("expected_outputs.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open expected_outputs.txt\n";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        expected_outputs.push_back(line);
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < expected_outputs.size(); ++idx) {
        std::string result;
        for (int i = 0; i < 8; ++i) {
            result += i_result.read();
        }
        bool passed = (result == expected_outputs[idx]);
        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: " << inputs[idx] << "\n";
            std::cout << "Output: " << result << "\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: " << inputs[idx] << "\n";
            std::cerr << "Output: " << result << ", Expected: " << expected_outputs[idx] << "\n";
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
