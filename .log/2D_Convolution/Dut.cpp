
#include "Dut.h"

#include <iostream>
#include <cstring>

extern "C" double* conv2D(
    const double* input, int in_h, int in_w,
    const double* kernel, int ker_h, int ker_w);

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
        int in_h = i_in_h.read();
        int in_w = i_in_w.read();
        int ker_h = i_ker_h.read();
        int ker_w = i_ker_w.read();
        sc_vector<sc_signal<double>> input_vec(in_h * in_w);
        sc_vector<sc_signal<double>> kernel_vec(ker_h * ker_w);
        for (int i = 0; i < in_h * in_w; ++i) {
            input_vec[i].write(i_input.read()[i]);
        }
        for (int i = 0; i < ker_h * ker_w; ++i) {
            kernel_vec[i].write(i_kernel.read()[i]);
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        double* input_arr = new double[in_h * in_w];
        double* kernel_arr = new double[ker_h * ker_w];
        for (int i = 0; i < in_h * in_w; ++i) {
            input_arr[i] = input_vec[i].read();
        }
        for (int i = 0; i < ker_h * ker_w; ++i) {
            kernel_arr[i] = kernel_vec[i].read();
        }
        double* output_arr = conv2D(input_arr, in_h, in_w, kernel_arr, ker_h, ker_w);
        /* === Main function Section End === */

        /* === Variable Section === */
        sc_vector<sc_signal<double>> output_vec(in_h * in_w);
        for (int i = 0; i < in_h * in_w; ++i) {
            output_vec[i].write(output_arr[i]);
        }
        o_output.write(output_vec);
        delete[] input_arr;
        delete[] kernel_arr;
        delete[] output_arr;
        /* === Variable Section End === */
    }
}
