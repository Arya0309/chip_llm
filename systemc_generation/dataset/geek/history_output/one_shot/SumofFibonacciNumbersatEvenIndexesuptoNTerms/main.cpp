
#include <systemc.h>

// Module to calculate sum of even-indexed Fibonacci numbers
SC_MODULE(FibonacciEvenSum) {
    sc_in<int> n;  // Input port for the number of terms
    sc_out<int> sum; // Output port for the sum of even-indexed Fibonacci numbers

    // Constructor
    SC_CTOR(FibonacciEvenSum) {
        // Process to calculate the sum
        SC_METHOD(calculate_even_sum);
        sensitive << n;
    }

    // Method to calculate the sum of even-indexed Fibonacci numbers
    void calculate_even_sum() {
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
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n; // Signal for the number of terms
    sc_signal<int> sum; // Signal for the sum of even-indexed Fibonacci numbers

    FibonacciEvenSum fib_even_sum_inst; // Instance of the FibonacciEvenSum module

    // Constructor
    SC_CTOR(Testbench) : fib_even_sum_inst("fib_even_sum") {
        // Connect signals to ports
        fib_even_sum_inst.n(n);
        fib_even_sum_inst.sum(sum);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set n to 8 and wait for the result
        n = 8;
        wait(1, SC_NS);
        cout << "Even indexed Fibonacci Sum upto " << n.read() << " terms: " << sum.read() << endl;

        // Additional test case: n = 5
        n = 5;
        wait(1, SC_NS);
        cout << "Even indexed Fibonacci Sum upto " << n.read() << " terms: " << sum.read() << endl;

        // Additional test case: n = 0 (edge case)
        n = 0;
        wait(1, SC_NS);
        cout << "Even indexed Fibonacci Sum upto " << n.read() << " terms: " << sum.read() << endl;

        // Additional test case: n = 1 (edge case)
        n = 1;
        wait(1, SC_NS);
        cout << "Even indexed Fibonacci Sum upto " << n.read() << " terms: " << sum.read() << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
