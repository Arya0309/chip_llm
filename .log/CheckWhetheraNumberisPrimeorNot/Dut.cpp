
#include "Dut.h"

bool is_prime(int n) {
    if (n <= 1)
        return false;

    int cnt = 0;

    // Check for divisors from 1 to n
    for (int i = 1; i <= n; i++) {
        // Check how many number is divisible
        // by n
        if (n % i == 0)
            cnt++;
    }

    // If n is divisible by more than 2 numbers
    // then it is not prime
    if (cnt > 2)
        return false;

    // else it is prime
    return true;
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
        bool res = is_prime(n);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
