
#include <systemc.h>
#include <cmath> // Include cmath for pow function in SystemC

// Module to compute power of two numbers
SC_MODULE(PowerCalculator) {
    sc_in<double> base;      // Input port for the base
    sc_in<double> exponent;  // Input port for the exponent
    sc_out<double> result;   // Output port for the result

    // Constructor
    SC_CTOR(PowerCalculator) {
        // Process to calculate power
        SC_METHOD(calculate_power);
        sensitive << base << exponent;
    }

    // Method to calculate the power
    void calculate_power() {
        result.write(pow(base.read(), exponent.read()));
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<double> base;     // Signal for the base
    sc_signal<double> exponent; // Signal for the exponent
    sc_signal<double> result;   // Signal for the result

    PowerCalculator power_inst; // Instance of the PowerCalculator module

    // Constructor
    SC_CTOR(Testbench) : power_inst("power_calc") {
        // Connect signals to ports
        power_inst.base(base);
        power_inst.exponent(exponent);
        power_inst.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set initial values
        base = 6.1;
        exponent = 4.8;
        wait(1, SC_NS); // Wait for the power calculation to process
        std::cout << "Result: " << result.read() << std::endl;

        // Additional test cases can be added here if needed

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
