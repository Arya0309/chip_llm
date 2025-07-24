#include "Dut.h"

constexpr int R1 = 2;
constexpr int C1 = 2;
constexpr int R2 = 2;
constexpr int C2 = 3;

void mulMat(int mat1[][C1], int mat2[][C2], int rslt[R1][C2]) {
    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            rslt[i][j] = 0;
            for (int k = 0; k < R2; k++) {
                rslt[i][j] += mat1[i][k] * mat2[k][j];
            }
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
        int mat1[R1][C1];
        int mat2[R2][C2];
        int rslt[R1][C2];

        // Read mat1 from FIFOs
        for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C1; j++) {
                mat1[i][j] = i_mat1.read();
            }
        }

        // Read mat2 from FIFOs
        for (int i = 0; i < R2; i++) {
            for (int j = 0; j < C2; j++) {
                mat2[i][j] = i_mat2.read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        mulMat(mat1, mat2, rslt);
        /* === Main function Section End === */

        /* === Variable Section === */
        // Write result matrix to FIFOs
        for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C2; j++) {
                o_result.write(rslt[i][j]);
            }
        }
        /* === Variable Section End === */
    }
}
