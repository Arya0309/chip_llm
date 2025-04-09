
#include <systemc.h>

// Module that calculates the GCD of two integers

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
        int x = a.read(); // Read the value of a
        int y = b.read(); // Read the value of b
        int res = std::min(x, y); // Initialize res to the smaller of x and y

        // Loop to find GCD
        while (res > 1) {
            if (x % res == 0 && y % res == 0)
                break;
            res--;
        }

        // Write the GCD result to the output port
        gcd_result.write(res);
    }
};

// Example top-level module to instantiate GcdCalculator

SC_MODULE(TopLevel) {
    sc_signal<int> a_sig; // Signal for input a
    sc_signal<int> b_sig; // Signal for input b
    sc_signal<int> gcd_res; // Signal for GCD result

    GcdCalculator gcd_inst; // Instance of GcdCalculator

    // Constructor
    SC_CTOR(TopLevel) : gcd_inst("GCDInstance") {
        // Connect signals to GcdCalculator ports
        gcd_inst.a(a_sig);
        gcd_inst.b(b_sig);
        gcd_inst.gcd_result(gcd_res);

        // Process to drive inputs and read output
        SC_THREAD(run);
    }

    // Thread to simulate input and check output
    void run() {
        // Example input values
        a_sig.write(12);
        b_sig.write(16);

        // Wait for the GCD calculation to complete
        wait(1, SC_NS);

        // Print the GCD result
        cout << "GCD of " << a_sig.read() << " and " << b_sig.read() << " is " << gcd_res.read() << endl;
    }
};

// Main simulation entry point


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
