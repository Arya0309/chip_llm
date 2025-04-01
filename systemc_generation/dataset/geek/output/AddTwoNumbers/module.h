#include <systemc.h>

// Module that adds two integers
SC_MODULE(Adder) {
    sc_in<int> a;  // Input port for the first number
    sc_in<int> b;  // Input port for the second number
    sc_out<int> sum; // Output port for the sum

    // Constructor
    SC_CTOR(Adder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << a << b;
    }

    // Method to add the two numbers
    void add() {
        sum.write(a.read() + b.read());
    }
};