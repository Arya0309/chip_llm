
#include <systemc.h>
#include <cmath> // Include cmath for pow function

// Module to calculate power of two numbers
SC_MODULE(PowerCalculator) {
    sc_in<double> base;       // Input port for the base
    sc_in<double> exponent;   // Input port for the exponent
    sc_out<double> result;    // Output port for the result

    // Constructor: Register the method to compute power
    SC_CTOR(PowerCalculator) {
        SC_METHOD(compute_power);
        sensitive << base << exponent;
    }

    // Method to compute the power
    void compute_power() {
        double x = base.read();
        double y = exponent.read();
        double res = std::pow(x, y); // Use std::pow from cmath
        result.write(res);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<double> base_sig;       // Signal for the base
    sc_signal<double> exponent_sig;   // Signal for the exponent
    sc_signal<double> result_sig;     // Signal for the result

    // Instance of the PowerCalculator module
    PowerCalculator power_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : power_inst("power_inst") {
        // Connect signals to module ports
        power_inst.base(base_sig);
        power_inst.exponent(exponent_sig);
        power_inst.result(result_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the base and exponent values
        base_sig.write(6.1);
        exponent_sig.write(4.8);
        wait(1, SC_NS); // Wait for the process to update

        // Read and print the result
        double result = result_sig.read();
        std::cout << "Result: " << result << std::endl;

        // End simulation after successful test
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
