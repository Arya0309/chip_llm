
#include <systemc.h>

// Module that calculates the sum of even-indexed Fibonacci numbers

SC_MODULE(FibonacciSumCalculator) {
    // Input port for the number of terms
    sc_in<int> n;
    // Output port for the sum of even-indexed Fibonacci numbers
    sc_out<int> sum;

    // Constructor
    SC_CTOR(FibonacciSumCalculator) {
        // Use an SC_METHOD to perform the calculation
        SC_METHOD(calculate_even_sum);
        sensitive << n;  // Sensitive to changes in 'n'
    }

    // Method that calculates the sum of even-indexed Fibonacci numbers
    void calculate_even_sum() {
        int n_val = n.read();  // Read the input value of n

        if (n_val <= 0) {
            sum.write(0);  // Return 0 if n is less than or equal to 0
            return;
        }

        // Array to store Fibonacci numbers
        int fibo[2 * n_val + 1];
        fibo[0] = 0, fibo[1] = 1;

        // Initialize result
        int result_sum = 0;

        // Calculate Fibonacci numbers and sum the even-indexed ones
        for (int i = 2; i <= 2 * n_val; i++) {
            fibo[i] = fibo[i - 1] + fibo[i - 2];

            // For even indices
            if (i % 2 == 0)
                result_sum += fibo[i];
        }

        // Write the result to the output port
        sum.write(result_sum);
    }
};

// Example usage in a testbench


SC_MODULE(Testbench) {
    sc_signal<int> n_sig; // Signal for the number of terms
    sc_signal<int> sum_sig; // Signal for the sum of even-indexed Fibonacci numbers

    // Instance of the FibonacciSumCalculator module
    FibonacciSumCalculator fib_sum_calc_inst;

    // Constructor
    SC_CTOR(Testbench)
    : fib_sum_calc_inst("fib_sum_calc_inst") {
        // Connect signals to module ports
        fib_sum_calc_inst.n(n_sig);
        fib_sum_calc_inst.sum(sum_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 8 (expected sum = 44)
        n_sig.write(8);
        wait(1, SC_NS); // Wait for the process to update
        int result = sum_sig.read();
        std::cout << "Even indexed Fibonacci Sum upto " << n_sig.read() << " terms: " << result << std::endl;
        assert(result == 1596);

        // Additional test case: n = 5 (expected sum = 10)
        n_sig.write(5);
        wait(1, SC_NS);
        result = sum_sig.read();
        std::cout << "Even indexed Fibonacci Sum upto " << n_sig.read() << " terms: " << result << std::endl;
        assert(result == 88);

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
