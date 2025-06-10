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
	o_a.reset();
    o_b.reset();
#endif

	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);

    int a, b;

    // Test case: a = 12, b = 16 (expected GCD = 4)
    a = 12;
    b = 16;
#ifndef NATIVE_SYSTEMC
    o_a.put(a);
    o_b.put(b);
#else
	o_a.write(a);
    o_b.write(b);
#endif
    std::cout << "case 1: GCD(" << a << ", " << b << ") " << std::endl;

    // Additional test case: a = 48, b = 18 (expected GCD = 6)
    a = 48;
    b = 18;
#ifndef NATIVE_SYSTEMC
    o_a.put(a);
    o_b.put(b);
#else
	o_a.write(a);
    o_b.write(b);
#endif
    std::cout << "case 2: GCD(" << a << ", " << b << ") " << std::endl;

}

void Testbench::do_fetch() {
#ifndef NATIVE_SYSTEMC
    i_result.reset();
#endif
    wait(5);
	wait(1);

    int result;
#ifndef NATIVE_SYSTEMC
	result = i_result.get();
#else
	result = i_result.read();
#endif
    std::cout << "ans 1: " << result << std::endl;
    assert(result == 4);

#ifndef NATIVE_SYSTEMC
	result = i_result.get();
#else
	result = i_result.read();
#endif
    std::cout << "ans 2: " << result << std::endl;
    assert(result == 6);

    std::cout << "All tests passed successfully." << std::endl;
    sc_stop();      // End simulation

}
