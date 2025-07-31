#include "Dut.h"

static double add(double a, double b) { return a + b; }

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
        double a = i_a.read();
        double b = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        double res = add(a, b);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}