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
    i_B.clk_rst(i_clk, i_rst);
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
        i_B.reset();
        o_C.reset();
        /* === Variable Section End === */
#endif
        wait();
    }

    while (true) {

        int A[4*4];
        int B[4*4];

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

        B[0] = i_B.get(); // Row 0, Column 0
        B[1] = i_B.get(); // Row 0, Column 1
        B[2] = i_B.get(); // Row 0, Column 2
        B[3] = i_B.get(); // Row 0, Column 3
        B[4] = i_B.get(); // Row 1, Column 0
        B[5] = i_B.get(); // Row 1, Column 1
        B[6] = i_B.get(); // Row 1, Column 2
        B[7] = i_B.get(); // Row 1, Column 3
        B[8] = i_B.get(); // Row 2, Column 0
        B[9] = i_B.get(); // Row 2, Column 1
        B[10] = i_B.get(); // Row 2, Column 2
        B[11] = i_B.get(); // Row 2, Column 3
        B[12] = i_B.get(); // Row 3, Column 0
        B[13] = i_B.get(); // Row 3, Column 1
        B[14] = i_B.get(); // Row 3, Column 2
        B[15] = i_B.get(); // Row 3, Column 3
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

        B[0] = i_B.read(); // Row 0, Column 0
        B[1] = i_B.read(); // Row 0, Column 1
        B[2] = i_B.read(); // Row 0, Column 2
        B[3] = i_B.read(); // Row 0, Column 3
        B[4] = i_B.read(); // Row 1, Column 0
        B[5] = i_B.read(); // Row 1, Column 1
        B[6] = i_B.read(); // Row 1, Column 2
        B[7] = i_B.read(); // Row 1, Column 3
        B[8] = i_B.read(); // Row 2, Column 0
        B[9] = i_B.read(); // Row 2, Column 1
        B[10] = i_B.read(); // Row 2, Column 2
        B[11] = i_B.read(); // Row 2, Column 3
        B[12] = i_B.read(); // Row 3, Column 0
        B[13] = i_B.read(); // Row 3, Column 1
        B[14] = i_B.read(); // Row 3, Column 2
        B[15] = i_B.read(); // Row 3, Column 3
        /* === Variable Section End === */
#endif

        /* === Main Function ===*/
        // For each row in Matrix A
        for (int i = 0; i < 4; i++) {
            // For each column in Matrix B
            for (int j = 0; j < 4; j++) {
                int sum = A[i*4+j]+B[i*4+j];
                // Write the result into the output matrix at position [i][j]
        /* === Main Function End === */
#ifndef NATIVE_SYSTEMC
                /* === Variable Section === */
                o_C.put(sum);
#else
                o_C.write(sum);
                /* === Variable Section End === */
#endif
            }
        }


    }

}
