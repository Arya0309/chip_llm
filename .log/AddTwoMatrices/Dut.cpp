
#include "Dut.h"

const int N = 4; // Define the size of the matrices

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
        int A[N][N];
        int B[N][N];
        int C[N][N];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i_A[i][j].read();
                B[i][j] = i_B[i][j].read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        add(A, B, C);
        /* === Main function Section End === */

        /* === Variable Section === */
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                o_C[i][j].write(C[i][j]);
            }
        }
        /* === Variable Section End === */
    }
}
