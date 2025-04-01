
#include <systemc.h>

// Module that multiplies two floating-point numbers
SC_MODULE(Multiplier) {
    sc_in<float> a;  // Input port for the first number
    sc_in<float> b;  // Input port for the second number
    sc_out<float> product; // Output port for the product

    // Constructor
    SC_CTOR(Multiplier) {
        // Process to perform multiplication
        SC_METHOD(multiply);
        sensitive << a << b;
    }

    // Method to multiply the two numbers
    void multiply() {
        product.write(a.read() * b.read());
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<float> a; // Signal for the first number
    sc_signal<float> b; // Signal for the second number
    sc_signal<float> product; // Signal for the product

    Multiplier multiplier_inst; // Instance of the Multiplier module

    // Constructor
    SC_CTOR(Testbench) : multiplier_inst("multiplier") {
        // Connect signals to ports
        multiplier_inst.a(a);
        multiplier_inst.b(b);
        multiplier_inst.product(product);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Simple multiplication
        a = 1.2; b = 3.0;
        wait(1, SC_NS); // Wait for the multiplier to process
        cout << "Product: " << product.read() << endl;

        // Additional test case: Different values
        a = 2.5; b = 4.0;
        wait(1, SC_NS);
        cout << "Product: " << product.read() << endl;

        // Additional test case: One operand is zero
        a = 0.0; b = 5.0;
        wait(1, SC_NS);
        cout << "Product: " << product.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
