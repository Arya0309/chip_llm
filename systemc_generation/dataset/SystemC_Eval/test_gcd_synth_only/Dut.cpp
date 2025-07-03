/* 
Instructions:
Fix Format: DO NOT MODIFY THIS SECTION
Variable Section: There are 2 input ports (i_a, i_b) and 1 output port (o_result) initially, change if needed.
Main function Section: Modify the function of the DUT here.
*/

#include "Dut.h"

int gcd(int a, int b) {
    // Find Minimum of a and b
    int res = std::min(a,b);

    // Testing divisibility with all numbers starting from
    // min(a, b) to 1
    while (res > 1) {
        // If any number divides both a and b, so we
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

    /* === Variable Section === */
	i_a.clk_rst(i_clk, i_rst);
    i_b.clk_rst(i_clk, i_rst);
    o_result.clk_rst(i_clk, i_rst);
    /* === Variable Section End === */
}

void Dut::do_compute() {

    {
        HLS_DEFINE_PROTOCOL("main_reset");
        /* === Variable Section === */
        i_a.reset();
        i_b.reset();
        o_result.reset();
        /* === Variable Section End === */
    }

    while (true) {
        /* === Variable Section === */
        int x = i_a.get();
        int y = i_b.get();
        /* === Variable Section End === */

        /* === Main function Section === */
        int res = gcd(x, y);
        /* === Main function Section End === */
        /* === Variable Section === */
		o_result.put(res);
        /* === Variable Section End === */
    }

}
