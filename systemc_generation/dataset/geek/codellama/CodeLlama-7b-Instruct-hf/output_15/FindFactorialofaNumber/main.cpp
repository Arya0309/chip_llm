scss
#include <systemc.h>

// Function to find factorial of a number
unsigned int factorial(unsigned int n);

// Function to calculate factorial of n
unsigned int calculateFactorial(unsigned int n) {
    int res = 1, i;
    for (i = 2; i <= n; i++)
        res *= i;
    return res;
}

// Module that simulates the factorial function

SC_MODULE(Factorial) {
    sc_in<unsigned int> in;  // Input port for the input value
    sc_out<unsigned int> out;  // Output port for the factorial value

    // Constructor
    SC_CTOR(Factorial) {
        // Calculate the factorial and store in the output
        SC_METHOD(factorial);
        sensitive << in;
        out << factorial(in.read());
    }
};

int main() {
    // Create an instance of the Factorial
    Factorial factorialModule;

    // Set the input value to calculate the factorial for
    factorialModule.in << 5;

    // Wait for the factorial to be calculated
    wait();

    // Print the result
    cout << "Factorial of " << factorialModule.in << " is "
         << factorialModule.out << endl;

    return 0;
}


SC_MODULE(Testbench) {
    sc_signal<unsigned int> num_sig;   // Signal to drive the input number
    sc_signal<unsigned int> fact_sig;  // Signal to capture the output factorial

    Factorial factorial_inst;  // Instance of the Factorial module

    // Constructor: initialize the instance and connect signals
    SC_CTOR(Testbench) : factorial_inst("factorial_inst") {
        factorial_inst.in(num_sig);
        factorial_inst.out(fact_sig);

        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that sets up test cases and checks the output
    void run_tests() {
        // Test case: Compute factorial for 5
        num_sig.write(5);
        wait(1, SC_NS);  // Wait for the process to execute

        // Read the computed factorial value
        unsigned int result = fact_sig.read();
        cout << "Factorial of 5 is " << result << endl;
        // Assert that the computed result is correct (120 for 5!)
        assert(result == 120);

        // Test case: Compute factorial for 0
        num_sig.write(0);
        wait(1, SC_NS);  // Wait for the process to execute
        result = fact_sig.read();
        cout << "Factorial of 0 is " << result << endl;
        // Assert that the computed result is correct (1 for 0!)
        assert(result == 1);

        std::cout << "All test passed successfully!" << std::endl;
        sc_stop();
    }
};

// sc_main: Entry point for the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");  // Create an instance of the Testbench module
    sc_start();          // Start the simulation
    return 0;
}
