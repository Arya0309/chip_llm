
#include "Dut.h"

void find_lcm(int a, int b) {
    int max_num, flag = 1;

    // Use ternary operator to get the
    // large number
    max_num = (a > b) ? a : b;

    while (flag) {
        // if statement checks max_num is completely
        // divisible by n1 and n2.
        if (max_num % a == 0 && max_num % b == 0) {
            cout << "LCM of " << a << " and " << b << " is "
                << max_num;
            break;
        }

        // update by 1 on each iteration
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
        find_lcm(a, b);
        /* === Main function Section End === */
    }
}
