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
	i_A.clk_rst(i_clk, i_rst);
    o_C.clk_rst(i_clk, i_rst);
    /* === Variable Section End === */
#endif

}

void Dut::do_compute() {

    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        /* === Variable Section === */
        i_A.reset();
        o_C.reset();
        /* === Variable Section End === */
#endif
        wait();
    }

    while (true) {

        int A[4*4];

#ifndef NATIVE_SYSTEMC
        /* === Variable Section === */
        A[0] = i_A.get(); // Row 0, Column 0
        A[1] = i_A.get(); // Row 0, Column 1
        A[2] = i_A.get(); // Row 0, Column 2
        A[3] = i_A.get(); // Row 0, Column 3
        A[4] = i_A.get(); // Row 1, Column 0
        A[5] = i_A.get(); // Row 1, Column 1
        A[6] = i_A.get(); // Row 1, Column 2
        A[7] = i_A.get(); // Row 1, Column 3
        A[8] = i_A.get(); // Row 2, Column 0
        A[9] = i_A.get(); // Row 2, Column 1
        A[10] = i_A.get(); // Row 2, Column 2
        A[11] = i_A.get(); // Row 2, Column 3
        A[12] = i_A.get(); // Row 3, Column 0
        A[13] = i_A.get(); // Row 3, Column 1
        A[14] = i_A.get(); // Row 3, Column 2
        A[15] = i_A.get(); // Row 3, Column 3
#else
        A[0] = i_A.read(); // Row 0, Column 0
        A[1] = i_A.read(); // Row 0, Column 1
        A[2] = i_A.read(); // Row 0, Column 2
        A[3] = i_A.read(); // Row 0, Column 3
        A[4] = i_A.read(); // Row 1, Column 0
        A[5] = i_A.read(); // Row 1, Column 1
        A[6] = i_A.read(); // Row 1, Column 2
        A[7] = i_A.read(); // Row 1, Column 3
        A[8] = i_A.read(); // Row 2, Column 0
        A[9] = i_A.read(); // Row 2, Column 1
        A[10] = i_A.read(); // Row 2, Column 2
        A[11] = i_A.read(); // Row 2, Column 3
        A[12] = i_A.read(); // Row 3, Column 0
        A[13] = i_A.read(); // Row 3, Column 1
        A[14] = i_A.read(); // Row 3, Column 2
        A[15] = i_A.read(); // Row 3, Column 3
        /* === Variable Section End === */
#endif

        /* === Main Function ===*/
        for (int i = 0; i < 4; i++) { 
            int t = A[i*4 + 0];
            A[i*4 + 0] = A[i*4 + 3];
            A[i*4 + 3] = t;
        }

        for (int idx = 0; idx < 16; ++idx) {
#ifndef NATIVE_SYSTEMC
            o_C.put(A[idx]);
#else
            o_C.write(A[idx]);
#endif
        }
    }
}
