
#include <systemc.h>


SC_MODULE(SimpleInterestCalculator) {
    // Input ports
    sc_in<float> P;  // Principal amount
    sc_in<float> R;  // Rate of interest
    sc_in<float> T;  // Time period

    // Output port
    sc_out<float> SI;  // Simple Interest

    // Constructor
    SC_CTOR(SimpleInterestCalculator) {
        // Process sensitivity declaration
        SC_METHOD(calculate);
        sensitive << P << R << T;
    }

    // Method to calculate simple interest
    void calculate() {
        float p = P.read();
        float r = R.read();
        float t = T.read();

        // Calculate simple interest
        float si = (p * t * r) / 100.0f;

        // Write the result to the output port
        SI.write(si);
    }
};

// Example top module for testing (not required for high-level synthesis)

SC_MODULE(TopModule) {
    // Instantiate the SimpleInterestCalculator
    SimpleInterestCalculator sic;

    // Signals for interconnection
    sc_signal<float> p;
    sc_signal<float> r;
    sc_signal<float> t;
    sc_signal<float> si;

    // Constructor
    SC_CTOR(TopModule) : sic("SimpleInterestCalculator") {
        // Connect signals to the SimpleInterestCalculator ports
        sic.P(p);
        sic.R(r);
        sic.T(t);
        sic.SI(si);

        // Initialize values for testing
        p.write(1.0f);
        r.write(1.0f);
        t.write(1.0f);

        // Trace signals for debugging (optional)
        // sc_trace_file *tf = sc_create_vcd_trace_file("simple_interest");
        // sc_trace(tf, p, "Principal");
        // sc_trace(tf, r, "Rate");
        // sc_trace(tf, t, "Time");
        // sc_trace(tf, si, "SimpleInterest");

        // End simulation after one delta cycle
        SC_THREAD(end_simulation);
    }

    // Thread to end simulation
    void end_simulation() {
        wait(1, SC_NS);  // Wait for one nanosecond
        sc_stop();        // Stop the simulation
    }
};

// Testbench entry point


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

        // Test case 2: P = 1000, R = 5, T = 2 (expected SI = 100.0)
        P_sig = 1000.0;
        R_sig = 5.0;
        T_sig = 2.0;
        wait(1, SC_NS); // Wait for the process to update
        result = SI_sig.read();
        std::cout << "Simple Interest for P=1000, R=5, T=2: " << result << std::endl;
        assert(abs(result - 100.0) < 1e-6); // Allow for floating-point precision errors
        
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
