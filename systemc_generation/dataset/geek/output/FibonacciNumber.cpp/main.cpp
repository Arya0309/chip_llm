
#include <systemc.h>

SC_MODULE(FibonacciCalculator) {
    sc_in<int> n;          // Input port for the position in the Fibonacci sequence
    sc_out<int> result;    // Output port for the Fibonacci number

    SC_CTOR(FibonacciCalculator) {
        SC_METHOD(calculate_fib);
        sensitive << n;
    }

    void calculate_fib() {
        int n_val = n.read(); // Read the input value
        int prev1 = 1, prev2 = 0, curr;

        if (n_val <= 1) {
            result.write(n_val); // Base cases: return n directly
        } else {
            for (int i = 2; i <= n_val; i++) {
                curr = prev1 + prev2;
                prev2 = prev1;
                prev1 = curr;
            }
            result.write(curr); // Write the result to the output port
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<int> n, result;

    FibonacciCalculator fib_calc("fib_calc");
    fib_calc.n(n);
    fib_calc.result(result);

    n.write(5); // Set the input value to 5
    sc_start(); // Start the simulation

    cout << "Fibonacci number at position " << n.read() << " is " << result.read() << endl;

    return 0;
}
