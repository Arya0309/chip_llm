
#include <systemc.h>

// Module to compute the nth Fibonacci number
SC_MODULE(Fibonacci) {
    sc_in<int> n;          // Input port for the position in the Fibonacci sequence
    sc_out<int> result;    // Output port for the nth Fibonacci number

    // Constructor
    SC_CTOR(Fibonacci) {
        // Process to compute the Fibonacci number
        SC_METHOD(compute_fib);
        sensitive << n;
    }

    // Method to compute the Fibonacci number
    void compute_fib() {
        int n_val = n.read();
        
        // Base case
        if (n_val <= 1) {
            result.write(n_val);
            return;
        }
        
        // Variables to store the last two Fibonacci numbers
        int prev1 = 1, prev2 = 0;
        int curr;

        // Loop to calculate the nth Fibonacci number
        for (int i = 2; i <= n_val; i++) {
            curr = prev1 + prev2;
            prev2 = prev1;
            prev1 = curr;
        }
        
        // Write the result to the output port
        result.write(curr);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n;         // Signal for the position in the Fibonacci sequence
    sc_signal<int> result;    // Signal for the nth Fibonacci number

    Fibonacci fib_inst;        // Instance of the Fibonacci module

    // Constructor
    SC_CTOR(Testbench) : fib_inst("fib_inst") {
        // Connect signals to ports
        fib_inst.n(n);
        fib_inst.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the position in the Fibonacci sequence
        n = 5;
        wait(1, SC_NS); // Wait for the Fibonacci module to process
        cout << "Fibonacci number at position " << n.read() << " is " << result.read() << endl;

        // Additional test cases
        n = 10;
        wait(1, SC_NS);
        cout << "Fibonacci number at position " << n.read() << " is " << result.read() << endl;

        n = 0;
        wait(1, SC_NS);
        cout << "Fibonacci number at position " << n.read() << " is " << result.read() << endl;

        n = 1;
        wait(1, SC_NS);
        cout << "Fibonacci number at position " << n.read() << " is " << result.read() << endl;

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
