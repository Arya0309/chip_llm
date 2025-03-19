
#include <systemc.h>

// Define a SystemC module for calculating simple interest
SC_MODULE(SimpleInterestCalculator) {
    // Constructor with sensitivity list (empty for this example)
    SC_CTOR(SimpleInterestCalculator) {
        SC_METHOD(calculate_interest);
    }

    // Method to calculate simple interest
    void calculate_interest() {
        // Hardcoded values for demonstration purposes
        float P = 1.0f; // Principal amount
        float R = 1.0f; // Rate of interest
        float T = 1.0f; // Time period

        // Calculate simple interest
        float SI = (P * T * R) / 100.0f;

        // Output the result
        sc_report_handler::report(sc_core::SC_INFO, "SIMPLE_INTEREST", ("Simple Interest = " + std::to_string(SI)).c_str());
    }
};

int sc_main(int argc, char* argv[]) {
    // Instantiate the SimpleInterestCalculator module
    SimpleInterestCalculator sic("SimpleInterestCalculator");

    // Start the simulation
    sc_start();

    return 0;
}
