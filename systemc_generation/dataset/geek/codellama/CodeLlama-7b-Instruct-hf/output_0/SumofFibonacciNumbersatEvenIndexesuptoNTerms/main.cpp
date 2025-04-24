scss
#include <systemc.h>

// Module that computes the even-indexed Fibonacci sum

SC_MODULE(FibonacciSumCalculator) {

    // Input signal for the number of Fibonacci terms
    sc_in<int> n;

    // Output signal for the sum of the first n even-indexed Fibonacci numbers
    sc_out<int> sum;

    // Constructor
    SC_CTOR(FibonacciSumCalculator) {

        // Process to compute the Fibonacci sum
        SC_METHOD(computeSum);
        sensitive << n;
    }

    // Method to compute the Fibonacci sum
    void computeSum() {

        // Array to hold the Fibonacci numbers
        int fibo[2 * n + 1];

        // Initialize the Fibonacci numbers
        fibo[0] = 0, fibo[1] = 1;

        // Initialize the sum
        int sum = 0;

        // Loop through the Fibonacci numbers
        for (int i = 2; i <= 2 * n; i++) {

            fibo[i] = fibo[i - 1] + fibo[i - 2];

            // For even indices, add the current Fibonacci number to the sum
            if (i % 2 == 0)
                sum += fibo[i];
        }

        // Return the sum
        sum.write(sum);
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
