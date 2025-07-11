#include "Dut.h"

#define N 4

void add(int A[][N], int B[][N], int C[][N]) {
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            C[i][j] = A[i][j] + B[i][j];
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
        int A[N][N], B[N][N], C[N][N];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i_a[i][j].read();
                B[i][j] = i_b[i][j].read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        add(A, B, C);
        /* === Main function Section End === */

        /* === Variable Section === */
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                o_result[i][j].write(C[i][j]);
            }
        }
        /* === Variable Section End === */
    }
}
