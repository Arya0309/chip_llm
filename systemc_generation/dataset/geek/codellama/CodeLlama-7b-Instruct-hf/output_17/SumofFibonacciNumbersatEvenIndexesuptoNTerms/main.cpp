scss
#include <systemc.h>

// Module that calculates the sum of the even-indiced FibonacciSumCalculator numbers

SC_MODULE(FibonacciSumCalculator) {
    sc_in<int> n;  // Input for the number of FibonacciSumCalculator terms
    sc_out<int> sum; // Output for the sum of the even-indiced FibonacciSumCalculator numbers

    // Constructor
    SC_CTOR(FibonacciSumCalculator) {}

    // Method to calculate the sum of the even-indiced FibonacciSumCalculator numbers
    void calculate_sum() {
        // Initialize an array to store the FibonacciSumCalculator sequence
        int fibo[n];
        fibo[0] = 0, fibo[1] = 1;

        // Iterate through the FibonacciSumCalculator sequence
        for (int i = 2; i <= n; i++) {
            // Add the current term to the FibonacciSumCalculator sequence
            fibo[i] = fibo[i - 1] + fibo[i - 2];

            // Check if the current term is even-indiced
            if (i % 2 == 0) {
                // Add the even-indiced term to the sum
                sum.write(sum.read() + fibo[i]);
            }
        }
    }
};


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
