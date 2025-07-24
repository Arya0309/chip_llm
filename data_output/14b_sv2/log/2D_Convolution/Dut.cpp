
#include "Dut.h"

constexpr int IN_H = 4;
constexpr int IN_W = 4;
constexpr int KER_H = 3;
constexpr int KER_W = 3;

// original convolution function
void conv2D(
    const double* input, int in_h, int in_w,
    const double* kernel, int ker_h, int ker_w,
    double* output)
{
    int pad_h = ker_h / 2;
    int pad_w = ker_w / 2;
    int padded_h = in_h + 2 * pad_h;
    int padded_w = in_w + 2 * pad_w;
    int out_h = in_h;
    int out_w = in_w;

    // dynamically allocate and initialize to 0
    double* padded = new double[padded_h * padded_w]();

    // copy original input to padded
    for (int i = 0; i < in_h; ++i) {
        for (int j = 0; j < in_w; ++j) {
            padded[(i + pad_h) * padded_w + (j + pad_w)] = input[i * in_w + j];
        }
    }

    // perform convolution
    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            double sum = 0.0;
            for (int m = 0; m < ker_h; ++m) {
                for (int n = 0; n < ker_w; ++n) {
                    sum += padded[(i + m) * padded_w + (j + n)]
                         * kernel[m * ker_w + n];
                }
            }
            output[i * out_w + j] = sum;
        }
    }

    delete[] padded;
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
        double input[IN_H * IN_W];
        double kernel[KER_H * KER_W];
        double output[IN_H * IN_W];

        // read input and kernel matrices element-by-element
        for (int i = 0; i < IN_H * IN_W; ++i) {
            input[i] = i_input.read();
        }
        for (int i = 0; i < KER_H * KER_W; ++i) {
            kernel[i] = i_kernel.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        conv2D(input, IN_H, IN_W, kernel, KER_H, KER_W, output);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result matrix back out
        for (int i = 0; i < IN_H * IN_W; ++i) {
            o_output.write(output[i]);
        }
        /* === Variable Section End === */
    }
}
