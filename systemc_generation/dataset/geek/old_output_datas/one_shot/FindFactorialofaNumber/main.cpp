
#include <systemc.h>

// Module to calculate factorial of a number
SC_MODULE(FactorialCalculator) {
    sc_in<unsigned int> input;  // Input port for the number
    sc_out<unsigned int> output; // Output port for the factorial result

    // Constructor
    SC_CTOR(FactorialCalculator) {
        // Process to calculate factorial
        SC_METHOD(calculate_factorial);
        sensitive << input;
    }

    // Method to calculate the factorial
    void calculate_factorial() {
        unsigned int n = input.read();
        unsigned int res = 1;
        for (unsigned int i = 2; i <= n; i++) {
            res *= i;
        }
        output.write(res);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<unsigned int> num; // Signal for the number
    sc_signal<unsigned int> fact; // Signal for the factorial result

    FactorialCalculator calc_inst; // Instance of the FactorialCalculator module

    // Constructor
    SC_CTOR(Testbench) : calc_inst("calc") {
        // Connect signals to ports
        calc_inst.input(num);
        calc_inst.output(fact);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Factorial of 5
        num = 5;
        wait(1, SC_NS); // Wait for the calculator to process
        assert(fact.read() == 120);
        cout << "Factorial of " << num.read() << " is " << fact.read() << endl;

        // Test case 2: Factorial of 0
        num = 0;
        wait(1, SC_NS);
        assert(fact.read() == 1);
        cout << "Factorial of " << num.read() << " is " << fact.read() << endl;

        // Test case 3: Factorial of 1
        num = 1;
        wait(1, SC_NS);
        assert(fact.read() == 1);
        cout << "Factorial of " << num.read() << " is " << fact.read() << endl;

        // Test case 4: Factorial of 10
        num = 10;
        wait(1, SC_NS);
        assert(fact.read() == 3628800);
        cout << "Factorial of " << num.read() << " is " << fact.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
