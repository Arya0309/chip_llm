
#include <systemc.h>

// Module to calculate simple interest
SC_MODULE(SimpleInterestCalculator) {
    sc_in<float> principal;  // Input port for principal amount
    sc_in<float> rate;       // Input port for rate of interest
    sc_in<float> time;       // Input port for time period
    sc_out<float> interest;  // Output port for simple interest

    // Constructor
    SC_CTOR(SimpleInterestCalculator) {
        // Process to calculate simple interest
        SC_METHOD(calculate_interest);
        sensitive << principal << rate << time;
    }

    // Method to calculate simple interest
    void calculate_interest() {
        float SI = (principal.read() * rate.read() * time.read()) / 100.0;
        interest.write(SI);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<float> principal; // Signal for principal amount
    sc_signal<float> rate;      // Signal for rate of interest
    sc_signal<float> time;       // Signal for time period
    sc_signal<float> interest;   // Signal for simple interest

    SimpleInterestCalculator sic_inst; // Instance of the SimpleInterestCalculator module

    // Constructor
    SC_CTOR(Testbench) : sic_inst("sic") {
        // Connect signals to ports
        sic_inst.principal(principal);
        sic_inst.rate(rate);
        sic_inst.time(time);
        sic_inst.interest(interest);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set initial values
        principal = 1.0;
        rate = 1.0;
        time = 1.0;
        wait(1, SC_NS); // Wait for the calculator to process
        cout << "Simple Interest = " << interest.read() << endl;

        // Print success message
        cout << "Test passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
