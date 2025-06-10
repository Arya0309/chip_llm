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
	o_A.reset();
    o_B.reset();
#endif

	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);


#ifndef NATIVE_SYSTEMC
    // Initialize Matrix A (2x2):
    // { {1, 1},
    //   {2, 2} }
    o_A.put(1); // Row 0, Column 0
    o_A.put(1); // Row 0, Column 1
    o_A.put(2); // Row 1, Column 0
    o_A.put(2); // Row 1, Column 1

    // Initialize Matrix B (2x3):
    // { {1, 1, 1},
    //   {2, 2, 2} }
    o_B.put(1); // Row 0, Column 0
    o_B.put(1); // Row 0, Column 1
    o_B.put(1); // Row 0, Column 2
    o_B.put(2); // Row 1, Column 0
    o_B.put(2); // Row 1, Column 1
    o_B.put(2); // Row 1, Column 2
#else
    // Initialize Matrix A (2x2):
    // { {1, 1},
    //   {2, 2} }
    o_A.write(1); // Row 0, Column 0
    o_A.write(1); // Row 0, Column 1
    o_A.write(2); // Row 1, Column 0
    o_A.write(2); // Row 1, Column 1

    // Initialize Matrix B (2x3):
    // { {1, 1, 1},
    //   {2, 2, 2} }
    o_B.write(1); // Row 0, Column 0
    o_B.write(1); // Row 0, Column 1
    o_B.write(1); // Row 0, Column 2
    o_B.write(2); // Row 1, Column 0
    o_B.write(2); // Row 1, Column 1
    o_B.write(2); // Row 1, Column 2
#endif
    
}

void Testbench::do_fetch() {
#ifndef NATIVE_SYSTEMC
    i_C.reset();
#endif
    wait(5);
	wait(1);

    // Expected result after multiplication:
    // For result[0][0]: 1*1 + 1*2 = 3
    // For result[0][1]: 1*1 + 1*2 = 3
    // For result[0][2]: 1*1 + 1*2 = 3
    // For result[1][0]: 2*1 + 2*2 = 6
    // For result[1][1]: 2*1 + 2*2 = 6
    // For result[1][2]: 2*1 + 2*2 = 6
    int expected[2*3] = {3, 3, 3, 6, 6, 6};

    std::cout << "Matrix multiplication result:" << std::endl;
    for (int i = 0; i < 2*3; i++) {
#ifndef NATIVE_SYSTEMC
	    int res = i_C.get();
#else
	    int res = i_C.read();
#endif
        std::cout << res << " ";
        // Assert the computed result matches the expected value
        assert(res == expected[i]);
    }
    std::cout << std::endl;
    std::cout << "All tests passed successfully." << std::endl;

    sc_stop(); // End simulation.

}
