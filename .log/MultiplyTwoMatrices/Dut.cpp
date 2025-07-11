#include "Dut.h"

void Dut::mulMat() {
    int rslt[R1][C2];

    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            rslt[i][j] = 0;

            for (int k = 0; k < R2; k++) {
                rslt[i][j] += i_mat1[i][k].read() * i_mat2[k][j].read();
            }

            o_result[i][j].write(rslt[i][j]);
        }
    }
}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(mulMat);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}
