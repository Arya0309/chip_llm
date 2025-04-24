systemc
#include <systemc.h>

// Module that calculates the gcd of two integers

SC_MODULE(GcdCalculator) {
    sc_in<int> a; // Input port for the first number
    sc_in<int> b; // Input port for the second number
    sc_out<int> gcd_result; // Output port for the gcd result

    // Constructor
    SC_CTOR(GcdCalculator) {
        // Process to calculate the gcd
        SC_METHOD(process);
        sensitive << a << b;
    }

    // Method that calculates the gcd
    void process() {
        int res; // Result of the gcd calculation

        // Find the minimum of a and b
        res = std::min(a.read(), b.read());

        // Testing divisibility with all numbers starting from
        // min(a, b) to 1
        while (res > 1) {

            // If any number divide both a and b, so we
            // got the answer
            if (a.read() % res == 0 && b.read() % res == 0)
                break;
            res--;
        }
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
