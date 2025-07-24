
#include "Dut.h"

constexpr int N = 4;

// original matrix-interchange function
void interchangeFirstLast(int m[][N]) {
    for(int i = 0; i < N; ++i) {
        int t = m[i][0];
        m[i][0] = m[i][N - 1];
        m[i][N - 1] = t;
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
        int m[N][N];
        // read matrix element-by-element
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                m[i][j] = i_m[i*N+j].read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        interchangeFirstLast(m);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result matrix back out
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                o_result[i*N+j].write(m[i][j]);
            }
        }
        /* === Variable Section End === */
    }
}
