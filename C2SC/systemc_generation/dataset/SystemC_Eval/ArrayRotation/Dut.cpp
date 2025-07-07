#include "Dut.h"

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */

#ifndef NATIVE_SYSTEMC
	i_d.clk_rst(i_clk, i_rst);
    i_in_array.clk_rst(i_clk, i_rst);
    o_out_array.clk_rst(i_clk, i_rst);
#endif

}

void Dut::do_compute() {

    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        i_d.reset();
        i_in_array.reset();
        o_out_array.reset();
#endif
        wait();
    }

    while (true) {

        // Read input array into a local array.
        int arr[7];
#ifndef NATIVE_SYSTEMC
        for (int i = 0; i < 7; i++) {
            arr[i] = i_in_array.get();
        }
        int rotation = i_d.get();
#else
        for (int i = 0; i < 7; i++) {
            arr[i] = i_in_array.read();
        }
        int rotation = i_d.read();
#endif

        // Temporary array to store rotated elements.
        int temp[7];
        int k = 0;

        // Copy elements from index 'rotation' to end.
        for (int i = rotation; i < 7; i++) {
            temp[k++] = arr[i];
        }
        // Copy first 'rotation' elements.
        for (int i = 0; i < rotation; i++) {
            temp[k++] = arr[i];
        }
        // Write the rotated array to output ports.
        for (int i = 0; i < 7; i++) {        
#ifndef NATIVE_SYSTEMC
		    o_out_array.put(temp[i]);
#else
		    o_out_array.write(temp[i]);
#endif
        }

    }

}
