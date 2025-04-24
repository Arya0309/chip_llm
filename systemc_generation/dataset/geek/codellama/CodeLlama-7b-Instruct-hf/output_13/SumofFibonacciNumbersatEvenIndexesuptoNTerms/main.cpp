c++
#include <systemc.h>

// Module for calculating the sum of 
// the even-indiced Fibonacci numbers

SC_MODULE(FibonacciSumCalculator)
{
    // Input port for the number of terms
    sc_in<int> n;

    // Output port for the sum
    sc_out<int> sum;

    // Function to compute the even-indiced 
    // Fibonacci numbers and return their sum
    void calculateEvenSum()
    {
        // Initialize the array of Fibonacci numbers
        int fibo[2 * n + 1] = {0};
        fibo[0] = 0;
        fibo[1] = 1;

        // Initialize the sum
        int sum = 0;

        // Iterate up to 2 * n
        for (int i = 2; i <= 2 * n; i++) 
        {
            // Compute the next Fibonacci number
            fibo[i] = fibo[i - 1] + fibo[i - 2];

            // If the index is even, add it to the sum
            if (i % 2 == 0)
                sum += fibo[i];
        }

        // Return the sum
        return sum;
    }

    // Create and bind the ports
    SC_CTOR(FibonacciSumCalculator)
    {
        // Create the input port
        bind(n)
            n.write();

        // Create the output port
        bind(sum)
            sum.write();
    }
};

// Test bench for the FibonacciSumCalculator module

SC_MODULE(FibonacciSumCalculatorTestBench)
{
    // Instantiate the module
    FibonacciSumCalculator sum;

    // Create and bind the ports
    void bind(sum)
    {
        // Create the input port
        sum.bind(n);

        // Create the output port
        sum.bind(sum);
    }

    // Test the module
    void run()
    {
        // Set the input values
        n.write(8);

        // Execute the module
        sum.calculateEvenSum();

        // Check the output value
        EXPECT_EQ(5, sum.read());
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
