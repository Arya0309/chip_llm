
#include <systemc.h>

// Module that swaps two integers

SC_MODULE(Swapper) {
    sc_in<int> a;       // Input port for the first number
    sc_in<int> b;       // Input port for the second number
    sc_out<int> out_a;  // Output port for the swapped first number
    sc_out<int> out_b;  // Output port for the swapped second number

    // Constructor
    SC_CTOR(Swapper) {
        // Process to perform swapping
        SC_METHOD(swap_values);
        sensitive << a << b;
    }

    // Method to swap the two numbers
    void swap_values() {
        int temp = a.read();  // Store the value of 'a' in 'temp'
        out_a.write(b.read()); // Write the value of 'b' to 'out_a'
        out_b.write(temp);     // Write the value of 'temp' (original 'a') to 'out_b'
    }
};

// Example top-level module to instantiate and run the Swapper

SC_MODULE(TopLevel) {
    sc_signal<int> a_sig;  // Signal for 'a'
    sc_signal<int> b_sig;  // Signal for 'b'
    sc_signal<int> out_a_sig; // Signal for 'out_a'
    sc_signal<int> out_b_sig; // Signal for 'out_b'

    Swapper swapper_inst; // Instance of the Swapper module

    // Constructor
    SC_CTOR(TopLevel) : swapper_inst("swapper_inst") {
        // Connect signals to the Swapper module
        swapper_inst.a(a_sig);
        swapper_inst.b(b_sig);
        swapper_inst.out_a(out_a_sig);
        swapper_inst.out_b(out_b_sig);

        // Process to initialize and monitor the signals
        SC_THREAD(initialize_and_monitor);
    }

    // Thread to initialize signals and monitor outputs
    void initialize_and_monitor() {
        // Initialize signals
        a_sig = 2;
        b_sig = 3;
        wait(1, SC_NS); // Wait for 1 nanosecond to allow the swapper to process

        // Print the values before and after swapping
        cout << "Before swapping a = " << a_sig.read() << " , b = " << b_sig.read() << endl;
        cout << "After swapping a = " << out_a_sig.read() << " , b = " << out_b_sig.read() << endl;
    }
};

// Main function to run the simulation


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
