#include "Dut.h"

int gcd(int a, int b) {
    // Find Minimum of a and b
    int res = std::min(a, b);

    // Testing divisibility with all numbers starting from
    // min(a, b) to 1

    while (res > 1) {

        // If any number divide both a and b, so we
        // got the answer
        if (a % res == 0 && b % res == 0)
            break;
        res--;
    }
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
        int a = i_a.read();
        int b = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        int res = gcd(a, b);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
