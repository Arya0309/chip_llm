#include "Dut.h"

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */

#ifndef NATIVE_SYSTEMC
	i_a.clk_rst(i_clk, i_rst);
    i_b.clk_rst(i_clk, i_rst);
    o_result.clk_rst(i_clk, i_rst);
#endif

}

void Dut::do_compute() {

    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        i_a.reset();
        i_b.reset();
        o_result.reset();
#endif
        wait();
    }

    while (true) {
#ifndef NATIVE_SYSTEMC
        int x = i_a.get();
        int y = i_b.get();
#else
        int x = i_a.read();
        int y = i_b.read();
#endif
        int res = (x < y) ? x : y; 
        while (res > 1) {
            if ((x % res == 0) && (y % res == 0))
                break;
            res--;
        }
#ifndef NATIVE_SYSTEMC
		o_result.put(res);
#else
		o_result.write(res);
#endif
    }

}
