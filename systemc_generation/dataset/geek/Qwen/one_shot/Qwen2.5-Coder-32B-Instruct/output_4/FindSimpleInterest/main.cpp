
#include <systemc.h>

// Module to calculate simple interest

SC_MODULE(SimpleInterestCalculator) {
    sc_in<float> P;  // Input port for Principal
    sc_in<float> R;  // Input port for Rate of Interest
    sc_in<float> T;  // Input port for Time
    sc_out<float> SI; // Output port for Simple Interest

    // Constructor
    SC_CTOR(SimpleInterestCalculator) {
        // Process to calculate simple interest
        SC_METHOD(calculate_interest);
        sensitive << P << R << T;
    }

    // Method to calculate simple interest
    void calculate_interest() {
        float si = (P.read() * T.read() * R.read()) / 100.0;
        SI.write(si);
    }
};

// Example top module to demonstrate usage

SC_MODULE(TopModule) {
    sc_signal<float> p_sig;
    sc_signal<float> r_sig;
    sc_signal<float> t_sig;
    sc_signal<float> si_sig;

    SimpleInterestCalculator sic_inst;

    SC_CTOR(TopModule) : sic_inst("SimpleInterestCalculator") {
        sic_inst.P(p_sig);
        sic_inst.R(r_sig);
        sic_inst.T(t_sig);
        sic_inst.SI(si_sig);

        // Initialize inputs
        p_sig = 1.0;
        r_sig = 1.0;
        t_sig = 1.0;

        // Process to monitor and display the output
        SC_METHOD(display_result);
        sensitive << si_sig;
    }

    void display_result() {
        cout << "Simple Interest = " << si_sig.read() << endl;
    }
};



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
