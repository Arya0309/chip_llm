
#include <systemc.h>

// Module to calculate factorial

SC_MODULE(Factorial) {
    sc_in<unsigned int> in;  // Input port for the number
    sc_out<unsigned int> out; // Output port for the factorial result

    // Constructor
    SC_CTOR(Factorial) {
        // Process to compute factorial
        SC_METHOD(compute_factorial);
        sensitive << in;
    }

    // Method to compute the factorial
    void compute_factorial() {
        unsigned int n = in.read();
        unsigned int res = 1;
        for (unsigned int i = 2; i <= n; ++i) {
            res *= i;
        }
        out.write(res);
    }
};

// Example top module for testing

SC_MODULE(Top) {
    Factorial calc;
    sc_signal<unsigned int> num;
    sc_signal<unsigned int> result;

    SC_CTOR(Top) : calc("calc") {
        calc.in(num);
        calc.out(result);

        // Initialize the number for which factorial is to be calculated
        SC_THREAD(initialize);
    }

    void initialize() {
        num.write(5); // Set the number to 5
        wait(1, SC_NS); // Wait for one nanosecond to see the result
        cout << "Factorial of " << num.read() << " is " << result.read() << endl;
        sc_stop(); // Stop the simulation
    }
};



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
