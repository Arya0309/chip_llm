#include <systemc.h>

// Module that adds two natural numbers
SC_MODULE(Adder) {
    sc_in<unsigned int> a;  // Input port for the first number
    sc_in<unsigned int> b;  // Input port for the second number
    sc_out<unsigned int> sum; // Output port for the sum

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

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<unsigned int> a; // Signal for the first number
    sc_signal<unsigned int> b; // Signal for the second number
    sc_signal<unsigned int> sum; // Signal for the sum

    Adder adder_inst; // Instance of the Adder module

    // Constructor
    SC_CTOR(Testbench) : adder_inst("adder") {
        // Connect signals to ports
        adder_inst.a(a);
        adder_inst.b(b);
        adder_inst.sum(sum);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Adding zero values
        // a = 0; b = 0;
        // wait(1, SC_NS); // Wait for the adder to process
        // assert(sum.read() == 0);
        // cout << sum.read() << endl; 

        // Test case 2: Simple addition
        a = 1; b = 2;
        wait(1, SC_NS);
        assert(sum.read() == 3);
        cout << sum.read() << endl;

        // Test case 3: Larger numbers
        a = 100; b = 200;
        wait(1, SC_NS);
        assert(sum.read() == 300);
        cout << sum.read() << endl;

        // Additional test case: One operand is zero
        a = 0; b = 5;
        wait(1, SC_NS);
        assert(sum.read() == 5);
        cout << sum.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        // User interaction simulation
        a = 3; b = 7; // Example user input
        wait(1, SC_NS);
        cout << "Sum: " << sum.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}