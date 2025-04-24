scss
#include <systemc.h>

// Module that calculates the alternating sum of
// even-indexed Fibonacci numbers up to n terms.

SC_MODULE(FibonacciSumCalculator) {
    sc_in<int> n; // Number of Fibonacci numbers to sum
    sc_out<int> sum; // Alternating sum of even-indexed Fibonacci numbers

    // Fibonacci numbers array
    int fibo[2*n+1];

    // Loop index
    int i;

    // Constructor
    SC_CTOR(FibonacciSumCalculator) {
        // Initialize fibo array with 0 and 1
        fibo[0] = 0;
        fibo[1] = 1;

        // Loop from 2 to 2*n-1
        for (i = 2; i <= 2*n-1; i++) {
            // Calculate fibonacci[i]
            fibo[i] = fibo[i-1] + fibo[i-2];
        }

        // Initialize sum to 0
        sum = 0;

        // Add remaining terms
        for (int i = 2; i <= 2*n; i++) {
            // Add to sum if i is even
            if (i % 2 == 0) {
                sum += fibo[i];
            }
        }

        // Return the alternating sum
        SC_METHOD(add);
        sensitive << n;
    }

    // Method to add the even-indexed Fibonacci numbers
    void add() {
        sum.write(sum.read() + fibo[n.read()]);
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
