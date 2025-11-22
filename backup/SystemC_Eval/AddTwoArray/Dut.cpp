#include "Dut.h"

constexpr int N = 4;

void add_arrays(const int A[N], const int B[N], int C[N]) {
    for (int i = 0; i < N; ++i) C[i] = A[i] + B[i];
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

    {
        wait();
    }

    while (true) {

        int a[N];
        int b[N];
        int c[N];

        for (int i = 0; i < N; i++) {
            a[i] = i_a.read(); // Read from input FIFO
            b[i] = i_b.read(); // Read from input FIFO
        }
        /* === Variable Section End === */

        /* === Main Function ===*/
        add_arrays(a, b, c);
        /* === Main Function End === */

        /* === Variable Section === */
        for (int i = 0; i < N; ++i) {
            o_c.write(c[i]);
        }
        /* === Variable Section End === */
    }
}
