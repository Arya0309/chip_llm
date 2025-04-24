c
#include <systemc.h>

// Module that swaps two integers

SC_MODULE(Swapper) {
    sc_in<int> a;  // Input port for the first number
    sc_in<int> b;  // Input port for the second number
    sc_out<int> swapped; // Output port for the swapped numbers

    // Constructor
    SC_CTOR(Swapper) {
        // Process to perform swapping
        SC_METHOD(swap);
        sensitive << a << b;
    }

    // Method to swap the two integers
    void swap() {
        int temp = a.read();
        a.write(b.read());
        b.write(temp);
        swapped.write(a.read() + b.read());
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> a;      // Signal for the first number.
    sc_signal<int> b;      // Signal for the second number.
    sc_signal<int> out_a;  // Signal for the swapped output of a.
    sc_signal<int> out_b;  // Signal for the swapped output of b.

    Swapper swapper_inst;  // Instance of the Swapper module.

    SC_CTOR(Testbench) : swapper_inst("swapper_inst") {
        // Connect the signals to the Swapper module ports.
        swapper_inst.a(a);
        swapper_inst.b(b);
        swapper_inst.out_a(out_a);
        swapper_inst.out_b(out_b);

        // Create a thread to run test cases.
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Set initial values (equivalent to a = 2, b = 3 in the C++ code).
        a.write(2);
        b.write(3);
        wait(1, SC_NS); // Allow time for the Swapper module to process the inputs.

        // Assert that the swap operation has been performed correctly.
        assert(out_a.read() == 3); // After swapping, out_a should be 3.
        assert(out_b.read() == 2); // After swapping, out_b should be 2.

        // Print the values before and after the swap.
        cout << "Before swapping a = " << a.read() << " , b = " << b.read() << endl;
        cout << "After swapping a = " << out_a.read() << " , b = " << out_b.read() << endl;
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation.
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the Testbench.
    sc_start();         // Start the simulation.
    return 0;
}
