#include "Dut.h"

constexpr int ROWS = 5;
constexpr int COLS = 6;

void median_filter(const int input[ROWS][COLS], int output[ROWS][COLS], int rows, int cols, int window_size) {
    int half = window_size / 2;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int count = 0;
            int* window = new int[window_size * window_size];
            // Gather values in the window
            for (int wi = i - half; wi <= i + half; ++wi) {
                for (int wj = j - half; wj <= j + half; ++wj) {
                    if (wi >= 0 && wi < rows && wj >= 0 && wj < cols) {
                        window[count++] = input[wi][wj];
                    }
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
            output[i][j] = window[count / 2];
            delete[] window;
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
        int input[ROWS][COLS];
        int output[ROWS][COLS];
        int window_size = i_window_size.read();
        // Read input matrix element-by-element
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                input[i][j] = i_input.read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        median_filter(input, output, ROWS, COLS, window_size);
        /* === Main function Section End === */

        /* === Variable Section === */
        // Write output matrix back out element-by-element
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                o_output.write(output[i][j]);
            }
        }
        /* === Variable Section End === */
    }
}
