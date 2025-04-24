
#include <systemc.h>

// Module that finds the sum of even-indexed Fibonacci numbers

SC_MODULE(FibonacciSumCalculator) {
    // Input/output ports
    sc_in<int> n;    // Number of Fibonacci numbers to be summed
    sc_out<int> sum; // Sum of even-indexed Fibonacci numbers up to n terms

    // Internal variables
    int nmax;        // Maximum term index
    int fibo[2 * nmax + 1]; // Fibonacci numbers array
    int sum;         // Even-indexed sum

    // Constructor
    SC_CTOR(FibonacciSumCalculator) {
        // Set up input and output ports
        n.bind(n);
        sum.bind(sum);

        // Initialize internal variables
        nmax = 0;
        sum = 0;
    }

    // Process to calculate the sum
    void calculateSum() {
        // Check if n is less than or equal to 0
        if (n <= 0) {
            sum = 0;
        } else {
            // Create the Fibonacci numbers array
            fibo[0] = 0;
            fibo[1] = 1;

            // Compute the remaining terms
            for (int i = 2; i <= 2 * n; i++) {
                fibo[i] = fibo[i - 1] + fibo[i - 2];

                // For even indices
                if (i % 2 == 0) {
                    sum += fibo[i];
                }
            }
        }

        // Output the sum
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
