
#include "Dut.h"

unsigned int factorial(unsigned int n) {
    unsigned int res = 1, i;
    for (i = 2; i <= n; i++)
        res *= i;
    return res;
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
        unsigned int n = i_n.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        unsigned int res = factorial(n);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
