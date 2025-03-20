
#include <systemc.h>

// Module to calculate simple interest
SC_MODULE(SimpleInterestCalculator) {
    sc_in<float> P;      // Input port for Principal
    sc_in<float> R;      // Input port for Rate
    sc_in<float> T;      // Input port for Time
    sc_out<float> SI;    // Output port for Simple Interest

    // Constructor
    SC_CTOR(SimpleInterestCalculator) {
        // Process to calculate simple interest
        SC_METHOD(calculate_si);
        sensitive << P << R << T;
    }

    // Method to calculate simple interest
    void calculate_si() {
        float principal = P.read();
        float rate = R.read();
        float time = T.read();
        float simple_interest = (principal * time * rate) / 100.0;
        SI.write(simple_interest);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<float> P_sig;      // Signal for Principal
    sc_signal<float> R_sig;      // Signal for Rate
    sc_signal<float> T_sig;      // Signal for Time
    sc_signal<float> SI_sig;     // Signal for Simple Interest

    SimpleInterestCalculator si_inst; // Instance of the SimpleInterestCalculator module

    // Constructor
    SC_CTOR(Testbench)
    : si_inst("si_inst") {
        // Connect signals to module ports
        si_inst.P(P_sig);
        si_inst.R(R_sig);
        si_inst.T(T_sig);
        si_inst.SI(SI_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: P = 1, R = 1, T = 1 (expected SI = 0.01)
        P_sig = 1.0;
        R_sig = 1.0;
        T_sig = 1.0;
        wait(1, SC_NS); // Wait for the process to update
        float result = SI_sig.read();
        std::cout << "Simple Interest for P=1, R=1, T=1: " << result << std::endl;
        assert(abs(result - 0.01) < 1e-6); // Allow for floating-point precision errors

        // End simulation after successful test
        std::cout << "Test passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
