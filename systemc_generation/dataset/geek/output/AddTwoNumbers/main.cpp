#include <systemc.h>
#include "module.h"

int sc_main(int argc, char* argv[]) {

    sc_signal<int> a, b, sum; // Signals for inputs and output
    Adder adder("Adder"); // Instantiate the Adder module
    adder.a(a); // Connect input a
    adder.b(b); // Connect input b
    adder.sum(sum); // Connect output sum

    // Initialize inputs
    a.write(5);
    b.write(3);
    sc_start(1, SC_NS);
    std::cout << a.read() << " + " << b.read() << " = " << sum.read() << std::endl;
    assert(sum.read() == 9);
    std::cout << "Unit test success" << std::endl;
    
    return 0;
}
