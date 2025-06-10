// #include <cassert>
// #include <cstdio>
// #include <cstdlib>
// using namespace std;

#include "Testbench.h"

/* === Fixed Format === */
Testbench::Testbench(sc_module_name n)
    : sc_module(n) {
    SC_THREAD(do_feed);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(do_fetch);
    sensitive << i_clk.pos();
    dont_initialize();
}
/* === Fixed Format End === */

void Testbench::do_feed() {

#ifndef NATIVE_SYSTEMC
	o_d.reset();
    o_in_array.reset();
#endif

	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);

    // Provide the input array and rotation offset (d = 2)
    int input[7] = { 1, 2, 3, 4, 5, 6, 7 };
#ifndef NATIVE_SYSTEMC
    for (int i = 0; i < 7; i++) {
        o_in_array.put(input[i]);
    }
    o_d.put(2);
#else
	for (int i = 0; i < 7; i++) {
        o_in_array.write(input[i]);
    }
    o_d.write(2);
#endif
    
}

void Testbench::do_fetch() {
#ifndef NATIVE_SYSTEMC
    i_out_array.reset();
#endif
    wait(5);
	wait(1);

    // Expected output after rotation: {3, 4, 5, 6, 7, 1, 2}
    int expected[7] = { 3, 4, 5, 6, 7, 1, 2 };
    std::cout << "Rotated Array: ";
    for (int i = 0; i < 7; i++) {
#ifndef NATIVE_SYSTEMC
	    int out_val = i_out_array.get();
#else
	    int out_val = i_out_array.read();
#endif
        // Assert that the output is as expected.
        assert(out_val == expected[i]);
        std::cout << out_val << " ";
    }
    std::cout << "All tests passed successfully." << std::endl;

    sc_stop(); // End simulation.

}
