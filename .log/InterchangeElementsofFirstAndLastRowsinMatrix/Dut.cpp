#include "Dut.h"

void interchangeFirstLast(int m[][4]) {
	// swapping of element between first
	// and last columns
	for (int i = 0; i < 4; i++) {
		int t = m[i][0];
		m[i][0] = m[i][3];
		m[i][3] = t;
	}
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
        int m[4][4];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = i * 4 + j;
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        interchangeFirstLast(m);
        /* === Main function Section End === */

        /* === Variable Section === */
        // Print the modified matrix for verification
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                std::cout << m[i][j] << " ";
            }
            std::cout << std::endl;
        }
        /* === Variable Section End === */
    }
}
