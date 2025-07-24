
#include "Dut.h"
#include <cmath>

bool isPrime(int n) {
    if (n <= 1)
        return false;

    for (int i = 2; i <= std::sqrt(n); i++) {
        if (n % i == 0)
            return false;
    }

    return true;
}

bool isPossible(int N) {
    // if the number is prime,
    // and number-2 is also prime
    if (isPrime(N) && isPrime(N - 2))
        return true;
    else
        return false;
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
        int N = i_input.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        bool result = isPossible(N);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_output.write(result);
        /* === Variable Section End === */
    }
}
