
#include "Dut.h"

void softmax(const double* input, double* output, int length) {
    // 找最大值以提升數值穩定性
    double max_val = input[0];
    for (int i = 1; i < length; ++i) {
        if (input[i] > max_val) {
            max_val = input[i];
        }
    }

    // 計算 exp(x - max_val) 並累加總和
    double sum = 0.0;
    for (int i = 0; i < length; ++i) {
        output[i] = exp(input[i] - max_val);
        sum += output[i];
    }

    // 正規化
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
        double input[5];
        for (int i = 0; i < 5; ++i) {
            input[i] = i_input[i].read();
        }
        double output[5];
        /* === Variable Section End === */

        /* === Main function Section === */
        softmax(input, output, 5);
        /* === Main function Section End === */

        /* === Variable Section === */
        for (int i = 0; i < 5; ++i) {
            o_output[i].write(output[i]);
        }
        /* === Variable Section End === */
    }
}
