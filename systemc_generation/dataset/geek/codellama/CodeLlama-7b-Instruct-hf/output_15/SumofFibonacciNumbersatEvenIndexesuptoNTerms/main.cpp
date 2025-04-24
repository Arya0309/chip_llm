scss
#include <systemc.h>


SC_MODULE(FibonacciSumCalculator)
{
    // Input and output ports
    sc_in<int> n;
    sc_out<int> sum;

    // Local member variables
    int fib[2 * n + 1];
    int sum_ = 0;

    // Function to calculate the Fibonacci sum
    void calculateFibSum(int);

    // Constructor
    FibonacciSumCalculator()
    {
        // Create a method to calculate the Fibonacci sum
        SC_METHOD(calculateFibSum);

        // Make the method sensitive to the input
        sensitive << n;
    }

    // Method to calculate the Fibonacci sum
    void calculateFibSum(int n)
    {
        // Initialize the fibonacci sequence
        fib[0] = 0;
        fib[1] = 1;

        // Iterate over the indices of the sequence
        for (int i = 2; i <= 2 * n; i++) 
        {
            // Calculate the next element of the sequence
            fib[i] = fib[i - 1] + fib[i - 2];

            // Check if the index is even
            if (i % 2 == 0)
            {
                // Count the even-indexed elements
                sum_ += fib[i];
            }
        }

        // Return the sum
        sum.write(sum_);
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
