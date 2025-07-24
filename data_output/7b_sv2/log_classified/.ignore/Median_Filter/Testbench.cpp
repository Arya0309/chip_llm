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
    constexpr int ROWS = 5;
    constexpr int COLS = 6;
    int input[ROWS][COLS] = {
        {1, 2, 3, 4, 5, 6},
        {7, 8, 9, 10, 11, 12},
        {13, 14, 15, 16, 17, 18},
        {19, 20, 21, 22, 23, 24},
        {25, 26, 27, 28, 29, 30}
    };

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            o_input.write(input[i][j]);
        }
    }
}

void Testbench::do_fetch() {
    constexpr int ROWS = 5;
    constexpr int COLS = 6;
    int expected[ROWS][COLS] = {
        {2, 3, 4, 5, 6, 7},
        {8, 9, 10, 11, 12, 13},
        {14, 15, 16, 17, 18, 19},
        {20, 21, 22, 23, 24, 25},
        {26, 27, 28, 29, 30, 31}
    };

    wait(1);

    bool all_passed = true;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            int result = i_output.read();
            bool passed = (result == expected[i][j]);
            if (!passed) {
                std::cerr << "Test case failed at row " << i << " col " << j << ". Expected: " << expected[i][j] << ", got: " << result << std::endl;
                all_passed = false;
            }
        }
    }

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
