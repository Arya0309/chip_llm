
#include "Dut.h"

constexpr int ROWS = 4;
constexpr int COLS = 4;

// original matrix column-interchange function
void interchangeFirstLast(int m[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        int t = m[i][0];
        m[i][0] = m[i][COLS - 1];
        m[i][COLS - 1] = t;
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
        int m[ROWS][COLS];
        // read matrix element-by-element
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                m[i][j] = i_matrix.read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        interchangeFirstLast(m);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result matrix back out
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                o_result.write(m[i][j]);
            }
        }
        /* === Variable Section End === */
    }
}
