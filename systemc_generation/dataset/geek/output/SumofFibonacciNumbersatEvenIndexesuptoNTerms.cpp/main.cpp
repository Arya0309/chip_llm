
#include <systemc.h>

// Module to calculate the sum of even-indexed Fibonacci numbers
SC_MODULE(FibonacciEvenSumCalculator) {
    sc_in<int> n_in;          // Input port for the number of terms
    sc_out<int> sum_out;      // Output port for the sum of even-indexed Fibonacci numbers

    // Process to calculate the sum
    void calculateEvenSum() {
        int n = n_in.read();    // Read the input value of n
        if (n <= 0) {
            sum_out.write(0);   // If n is non-positive, the sum is 0
            return;
        }

        int fibo[2 * n + 1];    // Array to store Fibonacci numbers
        fibo[0] = 0;            // First Fibonacci number
        fibo[1] = 1;            // Second Fibonacci number

        int sum = 0;            // Variable to accumulate the sum of even-indexed Fibonacci numbers

        // Calculate Fibonacci numbers and sum even-indexed ones
        for (int i = 2; i <= 2 * n; i++) {
            fibo[i] = fibo[i - 1] + fibo[i - 2];
            if (i % 2 == 0)
                sum += fibo[i];
        }

        sum_out.write(sum);     // Write the result to the output port
    }

    // Constructor to register the process
    SC_CTOR(FibonacciEvenSumCalculator) {
        SC_METHOD(calculateEvenSum);
        sensitive << n_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    sc_signal<int> n;         // Signal to connect with the FibonacciEvenSumCalculator
    sc_signal<int> sum;        // Signal to capture the output

    // Instance of FibonacciEvenSumCalculator
    FibonacciEvenSumCalculator calculator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        n = 8;                  // Set the input value of n
        wait(1, SC_NS);         // Wait for one delta cycle to let the calculator process

        // Print the result
        cout << "Even indexed Fibonacci Sum upto " << n.read() << " terms: " << sum.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : calculator("calculator") {
        // Connecting signals to the calculator ports
        calculator.n_in(n);
        calculator.sum_out(sum);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
