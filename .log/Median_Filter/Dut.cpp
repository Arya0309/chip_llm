
#include "Dut.h"

void median_filter(const int* input, int* output, int length, int window_size) {
    int half = window_size / 2;
    for (int i = 0; i < length; ++i) {
        int count = 0;
        // Allocate window buffer
        int* window = new int[window_size];
        // Gather values in the window
        for (int j = i - half; j <= i + half; ++j) {
            if (j >= 0 && j < length) {
                window[count++] = input[j];
            }
        }
        // Simple sorting (bubble sort) of the window
        for (int x = 0; x < count - 1; ++x) {
            for (int y = x + 1; y < count; ++y) {
                if (window[x] > window[y]) {
                    int tmp = window[x];
                    window[x] = window[y];
                    window[y] = tmp;
                }
            }
        }
        // Pick median
        output[i] = window[count / 2];
        delete[] window;
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
        int length = i_length.read();
        int window_size = i_window_size.read();
        const int* input = i_input.read().to_int_array();
        int* output = i_output.write().to_int_array();
        /* === Variable Section End === */

        /* === Main function Section === */
        median_filter(input, output, length, window_size);
        /* === Main function Section End === */
    }
}
