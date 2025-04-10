
#include <systemc.h>


SC_MODULE(FibonacciSumCalculator) {
    sc_in<int> n;
    sc_out<int> sum;

    void compute_even_sum() {
        int input_n = n.read();
        if (input_n <= 0) {
            sum.write(0);
            return;
        }

        int fibo[2 * input_n + 1];
        fibo[0] = 0, fibo[1] = 1;
        int even_sum = 0;

        for (int i = 2; i <= 2 * input_n; i++) {
            fibo[i] = fibo[i - 1] + fibo[i - 2];
            if (i % 2 == 0)
                even_sum += fibo[i];
        }

        sum.write(even_sum);
    }

    SC_CTOR(FibonacciSumCalculator) {
        SC_METHOD(compute_even_sum);
        sensitive << n;
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
