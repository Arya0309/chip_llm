
#include <systemc.h>

// Module to calculate compound interest
SC_MODULE(CompoundInterestCalculator) {
    sc_in<double> principal;  // Input port for principal amount
    sc_in<double> rate;       // Input port for interest rate
    sc_in<double> time;       // Input port for time period
    sc_out<double> CI;        // Output port for compound interest

    // Constructor
    SC_CTOR(CompoundInterestCalculator) {
        // Process to calculate compound interest
        SC_METHOD(calculate_interest);
        sensitive << principal << rate << time;
    }

    // Method to calculate compound interest
    void calculate_interest() {
        double A = principal.read() * pow((1 + rate.read() / 100), time.read());
        CI.write(A - principal.read());
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<double> principal; // Signal for principal amount
    sc_signal<double> rate;      // Signal for interest rate
    sc_signal<double> time;      // Signal for time period
    sc_signal<double> CI;         // Signal for compound interest

    CompoundInterestCalculator calc_inst; // Instance of the CompoundInterestCalculator module

    // Constructor
    SC_CTOR(Testbench) : calc_inst("calc") {
        // Connect signals to ports
        calc_inst.principal(principal);
        calc_inst.rate(rate);
        calc_inst.time(time);
        calc_inst.CI(CI);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Given values
        principal = 10000;
        rate = 5;
        time = 2;
        wait(1, SC_NS); // Wait for the calculator to process
        cout << "Compound interest is " << CI.read() << endl;

        // Additional test case
        principal = 15000;
        rate = 4.5;
        time = 3;
        wait(1, SC_NS);
        cout << "Compound interest is " << CI.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
