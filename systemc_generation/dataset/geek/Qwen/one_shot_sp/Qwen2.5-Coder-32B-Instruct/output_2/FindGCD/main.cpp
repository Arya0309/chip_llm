
#include <systemc.h>

// Module to calculate GCD of two integers

SC_MODULE(GcdCalculator) {
    sc_in<int> a;          // Input port for the first number
    sc_in<int> b;          // Input port for the second number
    sc_out<int> gcd_result; // Output port for the GCD result

    // Constructor
    SC_CTOR(GcdCalculator) {
        // Process to calculate GCD
        SC_METHOD(calculate_gcd);
        sensitive << a << b;
    }

    // Method to calculate the GCD
    void calculate_gcd() {
        int res = min(a.read(), b.read()); // Initialize res with the minimum of a and b
        while (res > 1) {
            if (a.read() % res == 0 && b.read() % res == 0) // Check divisibility
                break;
            res--; // Decrement res
        }
        gcd_result.write(res); // Write the GCD result
    }
};

// Example top-level module for simulation (not required for synthesis)

SC_MODULE(TopLevel) {
    GcdCalculator gcd_module;
    sc_signal<int> a_sig;
    sc_signal<int> b_sig;
    sc_signal<int> gcd_result_sig;

    SC_CTOR(TopLevel) : gcd_module("gcd_module") {
        gcd_module.a(a_sig);
        gcd_module.b(b_sig);
        gcd_module.gcd_result(gcd_result_sig);

        // Process to drive inputs and read output
        SC_THREAD(run);
    }

    void run() {
        a_sig.write(12);
        b_sig.write(16);
        wait(10, SC_NS); // Wait for some time to allow the calculation to complete
        cout << "GCD of " << a_sig.read() << " and " << b_sig.read() << " is: " << gcd_result_sig.read() << endl;
        sc_stop();
    }
};

// Main function for simulation


SC_MODULE(Testbench) {
    sc_signal<int> a_sig;       // Signal for the first input
    sc_signal<int> b_sig;       // Signal for the second input
    sc_signal<int> gcd_sig;     // Signal for the GCD output

    // Instance of the GcdCalculator module
    GcdCalculator gcd_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : gcd_inst("gcd_inst") {
        // Connect signals to module ports
        gcd_inst.a(a_sig);
        gcd_inst.b(b_sig);
        gcd_inst.gcd_result(gcd_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: a = 12, b = 16 (expected GCD = 4)
        a_sig.write(12);
        b_sig.write(16);
        wait(1, SC_NS); // Wait for the process to update
        int result = gcd_sig.read();
        std::cout << "GCD(" << a_sig.read() << ", " << b_sig.read() << ") = " << result << std::endl;
        assert(result == 4);

        // Additional test case: a = 48, b = 18 (expected GCD = 6)
        a_sig.write(48);
        b_sig.write(18);
        wait(1, SC_NS);
        result = gcd_sig.read();
        std::cout << "GCD(" << a_sig.read() << ", " << b_sig.read() << ") = " << result << std::endl;
        assert(result == 6);

        // End simulation after successful tests
        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
