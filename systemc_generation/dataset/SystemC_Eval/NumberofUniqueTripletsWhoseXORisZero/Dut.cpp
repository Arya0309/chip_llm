#include "Dut.h"

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */

#ifndef NATIVE_SYSTEMC
    i_in_array.clk_rst(i_clk, i_rst);
    o_out.clk_rst(i_clk, i_rst);
#endif

}

void Dut::do_compute() {

    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        i_in_array.reset();
        o_out.reset();
#endif
        wait();
    }

    while (true) {

        // Read input array into a local array.
        int arr[5];
#ifndef NATIVE_SYSTEMC
        for (int i = 0; i < 5; i++) {
            arr[i] = i_in_array.get();
        }
#else
        for (int i = 0; i < 5; i++) {
            arr[i] = i_in_array.read();
        }
#endif

        // To store values that are present
        int count = 0;
        for (int i = 0; i < 4; ++i) {
            for (int j = i+1; j < 5; ++j) {
                int xr = arr[i] ^ arr[j];
                // 線性搜尋剩下的 k
                for (int k = 0; k < 5; ++k) {
                    if (k==i || k==j) continue;
                    if (arr[k] == xr) {
                        ++count;
                    }
                }
            }
        }
        int result = count / 3;
    
#ifndef NATIVE_SYSTEMC
		    o_out.put(result);
#else
		    o_out.write(result);
#endif
    }

}
