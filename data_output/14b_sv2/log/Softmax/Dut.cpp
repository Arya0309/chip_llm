#include "Dut.h"
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

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        double input[LENGTH];
        for (int i = 0; i < LENGTH; ++i) {
            input[i] = i_input.read();
        }
        double output[LENGTH];
        /* === Variable Section End === */

        /* === Main function Section === */
        softmax(input, output, LENGTH);
        /* === Main function Section End === */

        /* === Variable Section === */
        for (int i = 0; i < LENGTH; ++i) {
            o_output.write(output[i]);
        }
        /* === Variable Section End === */
    }
}
