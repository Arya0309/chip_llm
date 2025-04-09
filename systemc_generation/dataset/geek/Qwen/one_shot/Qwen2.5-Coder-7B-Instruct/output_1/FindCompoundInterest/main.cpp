
#include <systemc.h>


SC_MODULE(CompoundInterestCalculator) {
    sc_in<double> principal;  // Input port for the principal amount
    sc_in<double> rate;       // Input port for the interest rate
    sc_in<double> time;       // Input port for the time period
    sc_out<double> ci;         // Output port for the compound interest

    // Constructor
    SC_CTOR(CompoundInterestCalculator) {
        // Process to calculate compound interest
        SC_METHOD(calculate_ci);
        sensitive << principal << rate << time;
    }

    // Method to calculate compound interest
    void calculate_ci() {
        double A = principal.read() * pow((1 + rate.read() / 100), time.read());
        double CI = A - principal.read();
        ci.write(CI);
    }
};

// Testbench module


SC_MODULE(Testbench) {
    sc_signal<double> principal_sig; // Signal for principal amount
    sc_signal<double> rate_sig;      // Signal for interest rate
    sc_signal<double> time_sig;      // Signal for time period
    sc_signal<double> ci_sig;        // Signal for compound interest

    // Instance of the CompoundInterestCalculator module
    CompoundInterestCalculator ci_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : ci_inst("ci_inst") {
        // Connect signals to module ports
        ci_inst.principal(principal_sig);
        ci_inst.rate(rate_sig);
        ci_inst.time(time_sig);
        ci_inst.ci(ci_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test Case 1: Principal = 10000, Rate = 5%, Time = 2 years
        principal_sig = 10000;
        rate_sig = 5;
        time_sig = 2;
        wait(1, SC_NS); // Wait for the process to update
        
        // Read and print the compound interest
        double result = ci_sig.read();
        // Check if the result is as expected (allowing a small tolerance)
        assert(fabs(result - 1025) < 1e-6);
        
        // Test Case 2: Principal = 20000, Rate = 10%, Time = 5 years
        principal_sig = 20000;
        rate_sig = 10;
        time_sig = 5;
        wait(1, SC_NS); // Wait for the process to update
        
        result = ci_sig.read(); // Reuse the already declared variable
        // The correct expected compound interest should be around 12210.2
        assert(fabs(result - 12210.2) < 1e-6);
        
        std::cout << "All tests passed successfully!" << std::endl;
        // End simulation after successful test
        sc_stop();
    }    
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
