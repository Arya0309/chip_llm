
#include <systemc.h>

// Module that swaps two integers using a third variable
SC_MODULE(Swapper) {
    sc_in<int> a_in;  // Input port for the first number
    sc_in<int> b_in;  // Input port for the second number
    sc_out<int> a_out; // Output port for the first number after swap
    sc_out<int> b_out; // Output port for the second number after swap

    // Constructor
    SC_CTOR(Swapper) {
        // Process to perform swapping
        SC_METHOD(swap_values);
        sensitive << a_in << b_in;
    }

    // Method to swap the two numbers
    void swap_values() {
        int temp;
        temp = a_in.read();
        a_out.write(b_in.read());
        b_out.write(temp);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a; // Signal for the first number
    sc_signal<int> b; // Signal for the second number
    sc_signal<int> a_swap; // Signal for the first number after swap
    sc_signal<int> b_swap; // Signal for the second number after swap

    Swapper swapper_inst; // Instance of the Swapper module

    // Constructor
    SC_CTOR(Testbench) : swapper_inst("swapper") {
        // Connect signals to ports
        swapper_inst.a_in(a);
        swapper_inst.b_in(b);
        swapper_inst.a_out(a_swap);
        swapper_inst.b_out(b_swap);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize values
        a = 2; b = 3;
        wait(1, SC_NS); // Wait for the swapper to process

        // Print initial values
        cout << "Before swapping a = " << a.read() << " , b = " << b.read() << endl;

        // Print swapped values
        cout << "After swapping a = " << a_swap.read() << " , b = " << b_swap.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
