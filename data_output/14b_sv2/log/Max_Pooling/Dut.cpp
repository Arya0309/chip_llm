
#include "Dut.h"

constexpr int IN_H = 4;
constexpr int IN_W = 4;
constexpr int POOL_H = 2;
constexpr int POOL_W = 2;
constexpr int STRIDE_H = 2;
constexpr int STRIDE_W = 2;
constexpr int OUT_H = (IN_H - POOL_H) / STRIDE_H + 1;
constexpr int OUT_W = (IN_W - POOL_W) / STRIDE_W + 1;

void maxPooling(const int* input,
                int in_h, int in_w,
                int pool_h, int pool_w,
                int stride_h, int stride_w,
                int* output,
                int& out_h, int& out_w) {
    out_h = (in_h - pool_h) / stride_h + 1;
    out_w = (in_w - pool_w) / stride_w + 1;

    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            int max_val = input[(i * stride_h) * in_w + (j * stride_w)];
            for (int m = 0; m < pool_h; ++m) {
                for (int n = 0; n < pool_w; ++n) {
                    int idx = (i * stride_h + m) * in_w + (j * stride_w + n);
                    if (input[idx] > max_val) {
                        max_val = input[idx];
                    }
                }
            }
            output[i * out_w + j] = max_val;
        }
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
        int input[IN_H * IN_W];
        for (int i = 0; i < IN_H * IN_W; ++i) {
            input[i] = i_input.read();
        }
        int output[OUT_H * OUT_W];
        int out_h, out_w;
        /* === Variable Section End === */

        /* === Main function Section === */
        maxPooling(input, IN_H, IN_W, POOL_H, POOL_W, STRIDE_H, STRIDE_W, output, out_h, out_w);
        /* === Main function Section End === */

        /* === Variable Section === */
        for (int i = 0; i < out_h * out_w; ++i) {
            o_output.write(output[i]);
        }
        /* === Variable Section End === */
    }
}
