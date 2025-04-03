
#include <systemc.h>

// Module to calculate the sum of first n natural numbers
SC_MODULE(SumCalculator) {
    sc_in<int> n;  // Input port for the number of natural numbers
    sc_out<int> sum; // Output port for the sum

    // Constructor
    SC_CTOR(SumCalculator) {
        // Process to calculate the sum
        SC_METHOD(calculate_sum);
        sensitive << n;
    }

    // Method to calculate the sum of first n natural numbers
    void calculate_sum() {
        int local_sum = 0;
        for (int i = 1; i <= n.read(); i++) {
            local_sum += i;
        }
        sum.write(local_sum);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n; // Signal for the number of natural numbers
    sc_signal<int> sum; // Signal for the sum

    SumCalculator calculator_inst; // Instance of the SumCalculator module

    // Constructor
    SC_CTOR(Testbench) : calculator_inst("calculator") {
        // Connect signals to ports
        calculator_inst.n(n);
        calculator_inst.sum(sum);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Calculate sum of first 5 natural numbers
        n = 5;
        wait(1, SC_NS); // Wait for the calculator to process
        assert(sum.read() == 15);
        cout << "Sum of first " << n.read() << " natural numbers: " << sum.read() << endl;

        // Test case 2: Calculate sum of first 10 natural numbers
        n = 10;
        wait(1, SC_NS);
        assert(sum.read() == 55);
        cout << "Sum of first " << n.read() << " natural numbers: " << sum.read() << endl;

        // Test case 3: Calculate sum of first 1 natural number
        n = 1;
        wait(1, SC_NS);
        assert(sum.read() == 1);
        cout << "Sum of first " << n.read() << " natural numbers: " << sum.read() << endl;

        // Test case 4: Calculate sum of first 0 natural numbers (edge case)
        n = 0;
        wait(1, SC_NS);
        assert(sum.read() == 0);
        cout << "Sum of first " << n.read() << " natural numbers: " << sum.read() << endl;

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
