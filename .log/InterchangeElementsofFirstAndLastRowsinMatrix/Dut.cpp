#include "Dut.h"

void Dut::interchangeFirstLast() {
    for (int i = 0; i < 4; i++) {
        int t = i_matrix[i][0];
        i_matrix[i][0] = i_matrix[i][3];
        i_matrix[i][3] = t;
    }
}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_interchange);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_interchange() {
    wait();
    while (true) {
        /* === Variable Section === */
        // Assuming i_matrix is already initialized and passed to the module
        /* === Variable Section End === */

        /* === Main function Section === */
        interchangeFirstLast();
        /* === Main function Section End === */

        /* === Variable Section === */
        // Assuming o_matrix is already initialized and passed to the module
        /* === Variable Section End === */
    }
}
