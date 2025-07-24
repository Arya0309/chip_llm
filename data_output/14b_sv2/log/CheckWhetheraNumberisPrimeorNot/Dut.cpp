
#include "Dut.h"

void check_prime(int n) {
    int cnt = 0;

    // If number is less than/equal to 1,
    // it is not prime
    if (n <= 1)
        o_result.write(false);
    else {

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
            o_result.write(false);

        // else it is prime
        else
            o_result.write(true);
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
        int n = i_n.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        check_prime(n);
        /* === Main function Section End === */
    }
}
