
#include "Dut.h"

void maxPooling(const int* input,
                int in_h, int in_w,
                int pool_h, int pool_w,
                int stride_h, int stride_w,
                int* output,
                int& out_h, int& out_w) {
    // 計算輸出維度
    out_h = (in_h - pool_h) / stride_h + 1;
    out_w = (in_w - pool_w) / stride_w + 1;

    // 對每個輸出位置執行 pooling
    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            int max_val = input[(i * stride_h) * in_w + (j * stride_w)];
            // 在 pooling 視窗內找最大值
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
        const int* input = i_input.read().data;
        int in_h = i_in_h.read();
        int in_w = i_in_w.read();
        int pool_h = i_pool_h.read();
        int pool_w = i_pool_w.read();
        int stride_h = i_stride_h.read();
        int stride_w = i_stride_w.read();
        int* output = i_output.read().data;
        int& out_h = i_out_h.read();
        int& out_w = i_out_w.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        maxPooling(input, in_h, in_w, pool_h, pool_w, stride_h, stride_w, output, out_h, out_w);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_out_h.write(out_h);
        o_out_w.write(out_w);
        /* === Variable Section End === */
    }
}
