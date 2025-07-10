
#include "Dut.h"

int fib(int n) {
    // For 0th and 1st term
    if (n <= 1)
        return n;

    // Variable to store the last two terms
    int prev1 = 1, prev2 = 0;

    // Variable that stores the current fibonacci term
    int curr;

    // Calculating the next fibonacci number by using
    // the previous two number
    for (int i = 2; i <= n; i++) {
        curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return curr;
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
        int n = i_n.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        int res = fib(n);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
