
#include <systemc.h>

// Module that calculates compound interest

SC_MODULE(CompoundInterestCalculator) {
    // Input ports
    sc_in<double> principal;
    sc_in<double> rate;
    sc_in<double> time;
    // Output port
    sc_out<double> ci;

    SC_CTOR(CompoundInterestCalculator) {
        // Sensitivity list for the method
        SC_METHOD(calculate_compound_interest);
        sensitive << principal << rate << time;
    }

    // Method to calculate compound interest
    void calculate_compound_interest() {
        // Read input values
        double p = principal.read();
        double r = rate.read();
        double t = time.read();

        // Calculate total amount A
        double A = p * (pow((1 + r / 100), t));
        // Calculate compound interest CI
        double CI = A - p;

        // Write the result to the output port
        ci.write(CI);
    }
};



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
