#include "Dut.h"

int calculate_lcm(int a, int b) {
    int max_num = (a > b) ? a : b;
    while (true) {
        if (max_num % a == 0 && max_num % b == 0) {
            return max_num;
        }
        ++max_num;
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
        int a = i_a.read();
        int b = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        int res = calculate_lcm(a, b);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
