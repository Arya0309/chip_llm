
#include <systemc.h>

// Module to calculate the nth Fibonacci number
SC_MODULE(FibonacciCalculator) {
    // Input port for the position in the Fibonacci sequence
    sc_in<int> n_in;
    
    // Output port for the nth Fibonacci number
    sc_out<int> fib_out;

    // Internal variables to store the last two Fibonacci numbers and the current one
    int prev1, prev2, curr;

    // Process to calculate the Fibonacci number
    void calculateFibonacci() {
        int n = n_in.read();
        
        // Base case handling
        if (n <= 1) {
            fib_out.write(n);
            return;
        }
        
        // Initialize the first two Fibonacci numbers
        prev1 = 1;
        prev2 = 0;
        
        // Calculate the nth Fibonacci number
        for (int i = 2; i <= n; i++) {
            curr = prev1 + prev2;
            prev2 = prev1;
            prev1 = curr;
        }
        
        // Write the result to the output port
        fib_out.write(curr);
    }

    // Constructor to register the process
    SC_CTOR(FibonacciCalculator) {
        SC_METHOD(calculateFibonacci);
        sensitive << n_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with FibonacciCalculator
    sc_signal<int> n_sig;
    sc_signal<int> fib_sig;

    // Instance of FibonacciCalculator
    FibonacciCalculator fibCalc;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Set the position in the Fibonacci sequence
        n_sig = 5;
        
        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);

        // Print the result
        cout << "The 5th Fibonacci number is: " << fib_sig.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : fibCalc("fibCalc") {
        // Connecting signals to the calculator ports
        fibCalc.n_in(n_sig);
        fibCalc.fib_out(fib_sig);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
