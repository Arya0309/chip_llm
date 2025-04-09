
#include <systemc.h>

// Module to calculate the nth Fibonacci number

SC_MODULE(FibonacciCalculator) {
    sc_in<int> n;         // Input port for the position in the Fibonacci sequence
    sc_out<int> result;   // Output port for the nth Fibonacci number

    // Constructor
    SC_CTOR(FibonacciCalculator) {
        // Process to calculate the Fibonacci number
        SC_METHOD(calculate_fib);
        sensitive << n;
    }

    // Method to calculate the Fibonacci number
    void calculate_fib() {
        int n_val = n.read(); // Read the input value

        // Handle base cases
        if (n_val <= 1) {
            result.write(n_val);
            return;
        }

        // Variables to store the last two terms
        int prev1 = 1, prev2 = 0;
        int curr;

        // Calculate the nth Fibonacci number
        for (int i = 2; i <= n_val; i++) {
            curr = prev1 + prev2;
            prev2 = prev1;
            prev1 = curr;
        }

        // Write the result to the output port
        result.write(curr);
    }
};

// Example top module for simulation

SC_MODULE(Top) {
    sc_signal<int> n;         // Signal for the position in the Fibonacci sequence
    sc_signal<int> result;    // Signal for the nth Fibonacci number

    FibonacciCalculator fib_calc; // Instance of the FibonacciCalculator module

    // Constructor
    SC_CTOR(Top) : fib_calc("fib_calc") {
        // Connect the signals to the FibonacciCalculator module
        fib_calc.n(n);
        fib_calc.result(result);

        // Process to drive the input and read the output
        SC_THREAD(run);
    }

    // Thread to drive the input and read the output
    void run() {
        // Test case: Calculate the 5th Fibonacci number
        n.write(5);
        wait(1, SC_NS); // Wait for the calculation to complete
        cout << "The 5th Fibonacci number is: " << result.read() << endl;

        // Additional test cases can be added here
    }
};

// Main function to initialize the simulation


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
