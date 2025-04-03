
#include <systemc.h>

// Module to swap two numbers using a third variable
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
        int a = a_in.read();
        int b = b_in.read();
        int temp;

        // Swapping algorithm
        temp = a;
        a = b;
        b = temp;

        a_out.write(a);
        b_out.write(b);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a_sig; // Signal for the first number
    sc_signal<int> b_sig; // Signal for the second number
    sc_signal<int> a_out_sig; // Signal for the first number after swap
    sc_signal<int> b_out_sig; // Signal for the second number after swap

    Swapper swapper_inst; // Instance of the Swapper module

    // Constructor
    SC_CTOR(Testbench) : swapper_inst("swapper") {
        // Connect signals to ports
        swapper_inst.a_in(a_sig);
        swapper_inst.b_in(b_sig);
        swapper_inst.a_out(a_out_sig);
        swapper_inst.b_out(b_out_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initial values
        a_sig = 2;
        b_sig = 3;

        // Wait for the swapper to process
        wait(1, SC_NS);

        // Print the values before and after swapping
        cout << "Before swapping a = " << a_sig.read() << " , b = " << b_sig.read() << endl;
        cout << "After swapping a = " << a_out_sig.read() << " , b = " << b_out_sig.read() << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
