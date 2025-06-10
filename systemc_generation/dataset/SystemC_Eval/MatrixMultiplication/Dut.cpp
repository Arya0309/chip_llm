#include "Dut.h"

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */

#ifndef NATIVE_SYSTEMC
	i_A.clk_rst(i_clk, i_rst);
    i_B.clk_rst(i_clk, i_rst);
    o_C.clk_rst(i_clk, i_rst);
#endif

}

void Dut::do_compute() {

    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        i_A.reset();
        i_B.reset();
        o_C.reset();
#endif
        wait();
    }

    while (true) {

        int A[2*2];
        int B[2*3];

#ifndef NATIVE_SYSTEMC
        A[0] = i_A.get(); // Row 0, Column 0
        A[1] = i_A.get(); // Row 0, Column 1
        A[2] = i_A.get(); // Row 1, Column 0
        A[3] = i_A.get(); // Row 1, Column 1

        B[0] = i_B.get(); // Row 0, Column 0
        B[1] = i_B.get(); // Row 0, Column 1
        B[2] = i_B.get(); // Row 0, Column 2
        B[3] = i_B.get(); // Row 1, Column 0
        B[4] = i_B.get(); // Row 1, Column 1
        B[5] = i_B.get(); // Row 1, Column 2
#else
        A[0] = i_A.read(); // Row 0, Column 0
        A[1] = i_A.read(); // Row 0, Column 1
        A[2] = i_A.read(); // Row 1, Column 0
        A[3] = i_A.read(); // Row 1, Column 1

        B[0] = i_B.read(); // Row 0, Column 0
        B[1] = i_B.read(); // Row 0, Column 1
        B[2] = i_B.read(); // Row 0, Column 2
        B[3] = i_B.read(); // Row 1, Column 0
        B[4] = i_B.read(); // Row 1, Column 1
        B[5] = i_B.read(); // Row 1, Column 2
#endif


        // For each row in Matrix A
        for (int i = 0; i < 2; i++) {
            // For each column in Matrix B
            for (int j = 0; j < 3; j++) {
                int sum = 0;
                // Compute dot product of row i of A and column j of B
                for (int k = 0; k < 2; k++) {
                    sum += A[i * 2 + k] * B[k * 3 + j];
                }
                // Write the result into the output matrix at position [i][j]
#ifndef NATIVE_SYSTEMC
                o_C.put(sum);
#else
                o_C.write(sum);
#endif
            }
        }


    }

}
