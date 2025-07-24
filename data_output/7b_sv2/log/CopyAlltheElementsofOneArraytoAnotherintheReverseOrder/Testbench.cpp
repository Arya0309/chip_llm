#include <systemc>
#include <iostream>
#include "Testbench.h"

using namespace sc_core;

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
    int input_array[] = {0, 1, 2, 3, 4, 5};
    for (int i = 0; i < N; ++i) {
        o_a.write(input_array[i]);
        wait();
    }
    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);
    sc_stop();
}

void Testbench::do_fetch() {
    int output_array[N];
    for (int i = 0; i < N; ++i) {
        output_array[i] = i_result.read();
        wait();
    }
    bool passed = true;
    for (int i = 0; i < N; ++i) {
        if (output_array[i] != 5 - i) {
            passed = false;
            break;
        }
    }
    if (passed) {
        std::cout << "Test passed." << std::endl;
    } else {
        std::cout << "Test failed." << std::endl;
    }
    sc_stop();
}
