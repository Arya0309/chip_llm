
#include "Dut.h"

#include <iostream>

/* === Fixed Format === */
SC_MODULE(Dut) {
    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_fifo_in<double*> i_input;
    sc_fifo_in<int> i_in_h;
    sc_fifo_in<int> i_in_w;
    sc_fifo_in<double*> i_kernel;
    sc_fifo_in<int> i_ker_h;
    sc_fifo_in<int> i_ker_w;
    sc_fifo_out<double*> o_output;

    SC_HAS_PROCESS(Dut);
    Dut(sc_module_name n) : sc_module(n) {
        SC_THREAD(do_conv2D);
        sensitive << i_clk.pos();
        dont_initialize();
        reset_signal_is(i_rst, false);
    }

    void do_conv2D() {
        wait();
        while (true) {
            /* === Variable Section === */
            double* input = i_input.read();
            int in_h = i_in_h.read();
            int in_w = i_in_w.read();
            double* kernel = i_kernel.read();
            int ker_h = i_ker_h.read();
            int ker_w = i_ker_w.read();
            /* === Variable Section End === */

            /* === Main function Section === */
            double* output = conv2D(input, in_h, in_w, kernel, ker_h, ker_w);
            /* === Main function Section End === */

            /* === Variable Section === */
            o_output.write(output);
            /* === Variable Section End === */
        }
    }

    double* conv2D(
        const double* input, int in_h, int in_w,
        const double* kernel, int ker_h, int ker_w)
    {
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
        return output;
    }
};
/* === Fixed Format End === */
