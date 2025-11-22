#include <systemc.h>
#include <cassert>
#include <iostream>

// Module to compute factorial of a given number
SC_MODULE(Factorial) {
    // Input port: receives the number whose factorial is to be computed
    sc_in<unsigned int> in;
    // Output port: outputs the computed factorial
    sc_out<unsigned int> out;

    // Process method to compute the factorial
    void compute() {
        unsigned int n = in.read();
        unsigned int result = 1;
        // Loop from 2 to n to calculate factorial
        for (unsigned int i = 2; i <= n; i++) {
            result *= i;
        }
        out.write(result);
    }

    // Constructor: declare the method process and sensitivity list
    SC_CTOR(Factorial) {
        SC_METHOD(compute);
        sensitive << in;
    }
};

// Testbench module for verifying the Factorial module
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
