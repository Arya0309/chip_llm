
#include "Dut.h"

bool isPrime(int n) {
    if (n <= 1)
        return false;

    int cnt = 0;

    for (int i = 1; i <= n; i++) {
        if (n % i == 0)
            cnt++;
    }

    return cnt <= 2;
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
        bool res = isPrime(n);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
