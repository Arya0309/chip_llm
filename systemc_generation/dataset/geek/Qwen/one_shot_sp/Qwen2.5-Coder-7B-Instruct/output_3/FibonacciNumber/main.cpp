
#include <systemc.h>


SC_MODULE(FibonacciCalculator) {
    sc_in<int> n;  // Input port for the position in the FibonacciCalculator sequence
    sc_out<int> result; // Output port for the FibonacciCalculator number

    // Constructor
    SC_CTOR(FibonacciCalculator) {
        // Process to calculate the FibonacciCalculator number
        SC_THREAD(calculate_fib);
        sensitive << n;
    }

    // Thread method to calculate the FibonacciCalculator number
    void calculate_fib() {
        if (n.read() <= 1) {
            result.write(n.read());
        } else {
            int prev1 = 1, prev2 = 0;
            int curr;
            for (int i = 2; i <= n.read(); i++) {
                curr = prev1 + prev2;
                prev2 = prev1;
                prev1 = curr;
            }
            result.write(curr);
        }
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> n_sig;          // Signal for the input position
    sc_signal<int> result_sig;      // Signal for the output Fibonacci number

    // Instance of the FibonacciCalculator module
    FibonacciCalculator fib_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : fib_inst("fib_inst") {
        // Connect signals to module ports
        fib_inst.n(n_sig);
        fib_inst.result(result_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 5 (expected Fibonacci number = 5)
        n_sig.write(5);
        wait(1, SC_NS); // Wait for the process to update
        int result = result_sig.read();
        std::cout << "Fibonacci number at position " << n_sig.read() << " is " << result << std::endl;
        assert(result == 5);

        // Additional test case: n = 10 (expected Fibonacci number = 55)
        n_sig.write(10);
        wait(1, SC_NS);
        result = result_sig.read();
        std::cout << "Fibonacci number at position " << n_sig.read() << " is " << result << std::endl;
        assert(result == 55);

        // End simulation after successful tests
        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
