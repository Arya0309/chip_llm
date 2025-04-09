
#include <systemc.h>

// Define a module named Factorial

SC_MODULE(Factorial) {
    // Input port for the number to calculate factorial
    sc_in<unsigned int> in;
    // Output port for the resulting factorial
    sc_out<unsigned int> out;

    // Constructor for the module
    SC_CTOR(Factorial) {
        // Sensitivity list: react to changes on the input signal
        SC_METHOD(calculate_factorial);
        sensitive << in;
    }

    // Method to calculate factorial
    void calculate_factorial() {
        unsigned int n = in.read();  // Read the input value
        unsigned int res = 1;       // Initialize result to 1
        for (unsigned int i = 2; i <= n; i++) {
            res *= i;               // Calculate factorial
        }
        out.write(res);             // Write the result to the output
    }
};

// Example testbench module to demonstrate usage


SC_MODULE(Testbench) {
    sc_signal<unsigned int> num_sig;   // Signal to drive the input number
    sc_signal<unsigned int> fact_sig;  // Signal to capture the output factorial

    Factorial factorial_inst;  // Instance of the Factorial module

    // Constructor: initialize the instance and connect signals
    SC_CTOR(Testbench) : factorial_inst("factorial_inst") {
        factorial_inst.in(num_sig);
        factorial_inst.out(fact_sig);

        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that sets up test cases and checks the output
    void run_tests() {
        // Test case: Compute factorial for 5
        num_sig.write(5);
        wait(1, SC_NS);  // Wait for the process to execute

        // Read the computed factorial value
        unsigned int result = fact_sig.read();
        cout << "Factorial of 5 is " << result << endl;
        // Assert that the computed result is correct (120 for 5!)
        assert(result == 120);

        // Test case: Compute factorial for 0
        num_sig.write(0);
        wait(1, SC_NS);  // Wait for the process to execute
        result = fact_sig.read();
        cout << "Factorial of 0 is " << result << endl;
        // Assert that the computed result is correct (1 for 0!)
        assert(result == 1);

        std::cout << "All test passed successfully!" << std::endl;
        sc_stop();
    }
};

// sc_main: Entry point for the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");  // Create an instance of the Testbench module
    sc_start();          // Start the simulation
    return 0;
}
