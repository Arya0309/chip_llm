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
    std::vector<int> input_data = {64, 34, 25, 12, 22, 11};
    std::vector<int> expected_output = {11, 12, 22, 25, 34, 64};

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (int data : input_data) {
        o_a.write(data);
        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<int> expected_output = {11, 12, 22, 25, 34, 64};

    wait(1);

    bool all_passed = true;
    for (int i = 0; i < expected_output.size(); ++i) {
        int result = i_result.read();
        if (result != expected_output[i]) {
            std::cerr << "Test case failed at index " << i << ". Expected: " << expected_output[i] << ", Got: " << result << std::endl;
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
