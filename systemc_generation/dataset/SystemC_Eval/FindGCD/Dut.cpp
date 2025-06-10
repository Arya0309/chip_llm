/* 
Instructions:
Fix Format: DO NOT MODIFY THIS SECTION
Variable Section: There are 2 input ports (i_a, i_b) and 1 output port (o_result) initially, change if needed.
Main function Section: Modify the function of the DUT here.
*/

#include "Dut.h"

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */

#ifndef NATIVE_SYSTEMC
    /* === Variable Section === */
	i_a.clk_rst(i_clk, i_rst);
    i_b.clk_rst(i_clk, i_rst);
    o_result.clk_rst(i_clk, i_rst);
    /* === Variable Section End === */
#endif

}

void Dut::do_compute() {

    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        /* === Variable Section === */
        i_a.reset();
        i_b.reset();
        o_result.reset();
        /* === Variable Section End === */
#endif
        wait();
    }

    while (true) {
#ifndef NATIVE_SYSTEMC
        /* === Variable Section === */
        int x = i_a.get();
        int y = i_b.get();
        /* === Variable Section End === */
#else
        /* === Variable Section === */
        int x = i_a.read();
        int y = i_b.read();
        /* === Variable Section End === */
#endif
        /* === Main function Section === */
        int res = (x < y) ? x : y;
        while (res > 1) {
            if ((x % res == 0) && (y % res == 0))
                break;
            res--;
        }
        /* === Main function Section End === */
#ifndef NATIVE_SYSTEMC
        /* === Variable Section === */
		o_result.put(res);
        /* === Variable Section End === */
#else
        /* === Variable Section === */
		o_result.write(res);
        /* === Variable Section End === */
#endif
    }

}
