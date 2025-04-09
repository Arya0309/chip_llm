
#include <systemc.h>

// Module that calculates the factorial of a number

SC_MODULE(Factorial) {
    sc_in<unsigned int> in;  // Input port for the number
    sc_out<unsigned int> out; // Output port for the factorial result

    // Constructor
    SC_CTOR(Factorial) {
        // Process to calculate factorial
        SC_METHOD(calculate_factorial);
        sensitive << in;
    }

    // Method to calculate the factorial
    void calculate_factorial() {
        unsigned int n = in.read();
        unsigned int res = 1;
        for (unsigned int i = 2; i <= n; i++) {
            res *= i;
        }
        out.write(res);
    }
};

// Example top-level module to instantiate and run the Factorial

SC_MODULE(TopLevel) {
    sc_signal<unsigned int> num; // Signal to connect to the input of Factorial
    sc_signal<unsigned int> fact_result; // Signal to capture the output of Factorial

    Factorial calc; // Instance of the Factorial module

    SC_CTOR(TopLevel) : calc("calc") {
        calc.in(num); // Connect the input signal to the Factorial's input
        calc.out(fact_result); // Connect the output signal to the Factorial's output

        // Process to drive the input and monitor the output
        SC_THREAD(run);
    }

    void run() {
        num.write(5); // Set the input to 5
        wait(1, SC_NS); // Wait for 1 nanosecond to allow the calculation to complete
        cout << "Factorial of " << num.read() << " is " << fact_result.read() << endl;
        sc_stop(); // Stop the simulation
    }
};

// Simulation entry point


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
