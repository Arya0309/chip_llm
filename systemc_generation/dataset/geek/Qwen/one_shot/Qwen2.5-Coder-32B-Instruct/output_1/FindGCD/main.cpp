
#include <systemc.h>

// Module that calculates the GCD of two numbers

SC_MODULE(GcdCalculator) {
    // Input ports for numbers a and b
    sc_in<int> a;
    sc_in<int> b;
    // Output port for the GCD result
    sc_out<int> gcd_result;

    // Constructor to register the process
    SC_CTOR(GcdCalculator) {
        // Use an SC_METHOD to perform the GCD calculation
        SC_METHOD(calculate_gcd);
        sensitive << a << b; // Trigger the method when a or b changes
    }

    // Method that calculates the GCD
    void calculate_gcd() {
        int num_a = a.read();
        int num_b = b.read();

        // Initialize the result to the smaller of num_a and num_b
        int res = (num_a < num_b) ? num_a : num_b;

        // Loop to find the GCD
        while (res > 1) {
            if (num_a % res == 0 && num_b % res == 0)
                break;
            res--;
        }

        // Write the result to the output port
        gcd_result.write(res);
    }
};



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
