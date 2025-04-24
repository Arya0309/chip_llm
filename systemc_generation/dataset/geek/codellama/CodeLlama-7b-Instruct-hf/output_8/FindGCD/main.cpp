scss
#include <systemc.h>

// Module that adds two integers

SC_MODULE(GcdCalculator) {
    sc_in<int> a;  // Input port for the first integer
    sc_in<int> b;  // Input port for the second integer
    sc_out<int> sum; // Output port for the sum

    // Constructor
    SC_CTOR(GcdCalculator) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << a << b;
    }

    // Method to add the two integers
    void add() {
        sum.write(a.read() + b.read());
    }
};

// Module that finds the GCD of two integers

SC_MODULE(GCD) {
    sc_in<int> a;  // Input port for the first integer
    sc_in<int> b;  // Input port for the second integer
    sc_out<int> gcd_result;  // Output port for the GCD

    // Constructor
    SC_CTOR(GCD) {
        // Process to find GCD
        SC_METHOD(find_gcd);
        sensitive << a << b;
    }

    // Method to find the GCD of two integers
    void find_gcd() {
        // Initialize the GCD with the minimum of the two integers
        int gcd = min(a.read(), b.read());

        // Looking for divisibility of all numbers starting from the minimum to 1
        while (gcd > 1) {
            // If any number divides both the first and second integer, so we got the answer
            if (a.read() % gcd == 0 && b.read() % gcd == 0)
                break;
            gcd--;
        }
        gcd_result.write(gcd);
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
