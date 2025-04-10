
#include <systemc.h>


SC_MODULE(GcdCalculator) {
    sc_in<int> a;          // Input signal for the first number
    sc_in<int> b;          // Input signal for the second number
    sc_out<int> gcd_result; // Output signal for the GcdCalculator result

    SC_CTOR(GcdCalculator) {
        SC_METHOD(calculate_gcd);
        sensitive << a << b; // Sensitivity list: react to changes in 'a' and 'b'
    }

    void calculate_gcd() {
        int num1 = a.read(); // Read the value of 'a'
        int num2 = b.read(); // Read the value of 'b'

        // Find Minimum of num1 and num2
        int res = std::min(num1, num2);

        // Testing divisibility with all numbers starting from min(num1, num2) to 1
        while (res > 1) {
            // If any number divides both num1 and num2, so we got the answer
            if (num1 % res == 0 && num2 % res == 0)
                break;
            res--;
        }

        // Write the result to the output signal
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
