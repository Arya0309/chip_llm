scss
#include <systemc.h>

// Define a sc_module for the even-indexed Fibonacci numbers

SC_MODULE(FibonacciSumCalculator) {
    // sc_in<int> n;  // Input port for n
    // sc_in<int> fibo[2 * n + 1]; // Input port for fibo
    // sc_out<int> sum;  // Output port for the sum

    // Constructor
    SC_CTOR(FibonacciSumCalculator) {
        // Register processes
        SC_METHOD(add);
    }

    // Function to add the two numbers
    void add() {
        // Loop through the Fibonacci numbers
        for (int i = 0; i < 2 * n + 1; i++) {
            // Update the current number
            fibo[i].write(fibo[i - 1] + fibo[i - 2]);

            // Sum up only the even-indexed numbers
            if (i % 2 == 0) {
                sum.write(sum.read() + fibo[i]);
            }
        }
    }
};

// Define the main function
int main() {
    // Create an instance of the Add module
    FibonacciSumCalculator add(n);

    // Connect the input and output ports
    add.n(n);
    add.fibo(fibo);
    add.sum(sum);

    // Run the simulation
    sc_start(10, SC_NS);

    // Print the results
    cout << "Even indexed Fibonacci sum upto " << n << " terms: " << sum << endl;

    return 0;
}


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
