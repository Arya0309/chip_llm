scss
#include <systemc.h>

// Module that computes the factorial of an input integer

SC_MODULE(Factorial) {
    sc_in<int> in; // Input port for the integer to be factored
    sc_out<int> out; // Output port for the factorial result

    // Constructor
    SC_CTOR(Factorial) {
        // Process to compute the factorial of in
        SC_METHOD(computeFactorial);
        sensitive << in;
    }

    // Method to compute the factorial of in
    void computeFactorial() {
        int res = 1, i;
        for (i = 2; i <= in; i++) {
            res *= i;
        }
        out.write(res);
    }
};

// Testbench for the Factorial module

SC_MODULE(FactorialTestbench) {
    sc_in<int> in;
    sc_out<int> out;

    // Instance of the Factorial module
    Factorial factorial;

    // Constructor
    SC_CTOR(FactorialTestbench) {
        factorial.in(in);
        factorial.out(out);
    }

    // Process to run the testbench
    void run_test() {
        int num = 5;
        in.write(num);
        wait(1); // Wait for the factorial calculation to complete
        unsigned int factorial_result = out.read();
        cout << "Factorial of " << num << " is " << factorial_result << endl;
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
