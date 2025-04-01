
#include <systemc.h>

// Module to compute the factorial of a number
SC_MODULE(FactorialCalculator) {
    sc_in<unsigned int> num_in;  // Input port for the number
    sc_out<unsigned int> fact_out; // Output port for the factorial

    // Constructor
    SC_CTOR(FactorialCalculator) {
        // Process to compute factorial
        SC_METHOD(compute_factorial);
        sensitive << num_in;
    }

    // Method to compute the factorial
    void compute_factorial() {
        unsigned int n = num_in.read();
        unsigned int res = 1;
        for (unsigned int i = 2; i <= n; i++) {
            res *= i;
        }
        fact_out.write(res);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<unsigned int> num_sig; // Signal for the number
    sc_signal<unsigned int> fact_sig; // Signal for the factorial

    FactorialCalculator fact_inst; // Instance of the FactorialCalculator module

    // Constructor
    SC_CTOR(Testbench) : fact_inst("fact_inst") {
        // Connect signals to ports
        fact_inst.num_in(num_sig);
        fact_inst.fact_out(fact_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the number to compute factorial
        num_sig.write(5);
        wait(1, SC_NS); // Wait for the process to update
        unsigned int result = fact_sig.read();
        cout << "Factorial of " << num_sig.read() << " is " << result << endl;

        // End simulation after successful test
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
