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
    std::vector<unsigned char> input;
    std::ifstream fin("input.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open input.txt" << std::endl;
        sc_stop();
        return;
    }
    unsigned char pixel;
    while (fin >> pixel) {
        input.push_back(pixel);
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto& pixel : input) {
        o_input.write(pixel);
        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<unsigned char> expected;
    std::ifstream fin("expected.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open expected.txt" << std::endl;
        sc_stop();
        return;
    }
    unsigned char pixel;
    while (fin >> pixel) {
        expected.push_back(pixel);
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < expected.size(); ++idx) {
        unsigned char result = i_output.read();
        bool passed = (result == expected[idx]);
        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed." << std::endl;
        } else {
            std::cerr << "Test case " << idx + 1 << " failed. Expected: " << static_cast<int>(expected[idx]) << ", but got: " << static_cast<int>(result) << std::endl;
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
