
#include "Dut.h"

constexpr int IN_H = 4;
constexpr int IN_W = 4;
constexpr int KER_H = 3;
constexpr int KER_W = 3;

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
        // read input matrix element-by-element
        for (int i = 0; i < IN_H; ++i) {
            for (int j = 0; j < IN_W; ++j) {
                input[i * IN_W + j] = i_input.read();
            }
        }
        // read kernel matrix element-by-element
        for (int i = 0; i < KER_H; ++i) {
            for (int j = 0; j < KER_W; ++j) {
                kernel[i * KER_W + j] = i_kernel.read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        double output[IN_H * IN_W];
        conv2D(input, IN_H, IN_W, kernel, KER_H, KER_W, output);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result matrix back out
        for (int i = 0; i < IN_H; ++i) {
            for (int j = 0; j < IN_W; ++j) {
                o_output.write(output[i * IN_W + j]);
            }
        }
        /* === Variable Section End === */
    }
}

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

    // 動態配置並初始化為 0
    double* padded = new double[padded_h * padded_w]();

    // 將原始輸入複製到 padded 內
    for (int i = 0; i < in_h; ++i) {
        for (int j = 0; j < in_w; ++j) {
            padded[(i + pad_h) * padded_w + (j + pad_w)] = input[i * in_w + j];
        }
    }

    // 執行卷積
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
