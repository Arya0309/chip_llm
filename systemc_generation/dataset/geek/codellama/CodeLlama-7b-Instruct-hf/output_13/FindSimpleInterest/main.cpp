sc_ldf
#include <systemc.h>


SC_MODULE(SimpleInterestCalculator) {
    sc_in<float> P;  // Principal amount 
  sc_in<float>R; // Rate of interest
  sc_in<float>T;// Time
  sc_out<float>SI;//Simple interest

  void do_calculate();   // Analog for main 

  // Constructor
  SC_CTOR(SimpleInterestCalculator) {
      // Process to find simple interest
      SC_METHOD(do_calculate); 
     SC_THREAD_forward_event<SC_POOL>; 
  }

  // Run_method
  void do_calculate() {
sc_time delay_string;
    std::string input;
    cout << " Please enter principal amount: ";
    cin >> P;
    cout << " Please enter rate of interest : ";
    cin >> R;
    cout << " Please enter time  : ";
    cin >> T;

    SI = (P * T * R) / 100.0;

    cout << "Simple interest = ";
    cout << SI;
cout << endl;

    } // Close do_calculate()
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
