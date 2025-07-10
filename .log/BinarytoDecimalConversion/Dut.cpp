
#include "Dut.h"

int binaryToDecimal(int n) {
    int num = n;
    int dec_value = 0;

    // Initializing base value to 
    // 1, i.e 2^0
    int base = 1;

    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;
        dec_value += last_digit * base;
        base = base * 2;
    }

    return dec_value;
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
        int dec_value = binaryToDecimal(n);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_dec_value.write(dec_value);
        /* === Variable Section End === */
    }
}
