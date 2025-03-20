
#include <systemc.h>

// Module to calculate power of a number
SC_MODULE(PowerCalculator) {
    sc_in<int> base;           // Input port for the base
    sc_in<int> exponent;      // Input port for the exponent
    sc_out<int> result;        // Output port for the result (power)

    // Constructor: Register the method to compute power
    SC_CTOR(PowerCalculator) {
        SC_METHOD(compute_power);
        sensitive << base << exponent;
    }

    // Method to compute the power
    void compute_power() {
        int a = base.read();
        int b = exponent.read();
        if (b == 0) {
            result.write(1);
            return;
        }
        int answer = a;
        int increment = a;
        for (int i = 1; i < b; i++) {
            for (int j = 1; j < a; j++) {
                answer += increment;
            }
            increment = answer;
        }
        result.write(answer);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> base_sig;   // Signal for the base input
    sc_signal<int> exp_sig;    // Signal for the exponent input
    sc_signal<int> res_sig;    // Signal for the result output

    // Instance of the PowerCalculator module
    PowerCalculator power_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : power_inst("power_inst") {
        // Connect signals to module ports
        power_inst.base(base_sig);
        power_inst.exponent(exp_sig);
        power_inst.result(res_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: base = 5, exponent = 3 (expected result = 125)
        base_sig.write(5);
        exp_sig.write(3);
        wait(1, SC_NS); // Wait for the process to update
        int result = res_sig.read();
        std::cout << "Power(" << base_sig.read() << ", " << exp_sig.read() << ") = " << result << std::endl;
        assert(result == 125);

        // Test case: base = 2, exponent = 4 (expected result = 16)
        base_sig.write(2);
        exp_sig.write(4);
        wait(1, SC_NS); // Wait for the process to update
        result = res_sig.read();
        std::cout << "Power(" << base_sig.read() << ", " << exp_sig.read() << ") = " << result << std::endl;
        assert(result == 16);

        // Test case: base = 3, exponent = 0 (expected result = 1)
        base_sig.write(3);
        exp_sig.write(0);
        wait(1, SC_NS); // Wait for the process to update
        result = res_sig.read();
        std::cout << "Power(" << base_sig.read() << ", " << exp_sig.read() << ") = " << result << std::endl;
        assert(result == 1);

        // Test case: base = 0, exponent = 5 (expected result = 0)
        base_sig.write(0);
        exp_sig.write(5);
        wait(1, SC_NS); // Wait for the process to update
        result = res_sig.read();
        std::cout << "Power(" << base_sig.read() << ", " << exp_sig.read() << ") = " << result << std::endl;
        assert(result == 0);

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
