#include "Dut.h"

void Dut::conv2D() {
    int pad_h = ker_h / 2;
    int pad_w = ker_w / 2;
    int padded_h = in_h + 2 * pad_h;
    int padded_w = in_w + 2 * pad_w;
    int out_h = in_h;
    int out_w = in_w;

    // 動態配置並初始化為 0
    double* padded = new double[padded_h * padded_w]();
    double* output = new double[out_h * out_w];

    // 將原始輸入複製到 padded 內
    for (int i = 0; i < in_h; ++i) {
        for (int j = 0; j < in_w; ++j) {
            padded[(i + pad_h) * padded_w + (j + pad_w)] = i_input[i * in_w + j].read();
        }
    }

    // 執行卷積
    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            double sum = 0.0;
            for (int m = 0; m < ker_h; ++m) {
                for (int n = 0; n < ker_w; ++n) {
                    sum += padded[(i + m) * padded_w + (j + n)]
                         * i_kernel[m * ker_w + n].read();
                }
            }
            o_output[i * out_w + j].write(sum);
        }
    }

    delete[] padded;
}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(conv2D);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}
