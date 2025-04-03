
#include <systemc.h>

// Module to print divisors of a given number
SC_MODULE(DivisorPrinter) {
    sc_in<int> number;  // Input port for the number
    sc_out<sc_bv<10>> divisors; // Output port for the divisors (as a bit vector for demonstration)

    // Constructor
    SC_CTOR(DivisorPrinter) {
        // Process to print divisors
        SC_METHOD(print_divisors);
        sensitive << number;
    }

    // Method to print divisors
    void print_divisors() {
        int n = number.read();
        sc_bv<10> divisors_output = 0; // Bit vector to store divisors (for demonstration purposes)

        cout << "The divisors of " << n << " are: ";
        for (int i = 1; i <= n; i++) {
            if (n % i == 0) {
                cout << i << " ";
                // For demonstration, set the corresponding bit in the bit vector
                if (i <= 10) { // Limiting to first 10 bits for simplicity
                    divisors_output[i-1] = 1;
                }
            }
        }
        cout << endl;
        divisors.write(divisors_output);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> number_sig; // Signal for the number
    sc_signal<sc_bv<10>> divisors_sig; // Signal for the divisors

    // Instance of the DivisorPrinter module
    DivisorPrinter divisor_printer_inst;

    // Constructor
    SC_CTOR(Testbench) : divisor_printer_inst("divisor_printer") {
        // Connect signals to ports
        divisor_printer_inst.number(number_sig);
        divisor_printer_inst.divisors(divisors_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: Number 100
        number_sig.write(100);
        wait(1, SC_NS); // Wait for the divisor printer to process
        cout << "Divisors bit vector: " << divisors_sig.read() << endl;

        // Additional test case: Number 30
        number_sig.write(30);
        wait(1, SC_NS);
        cout << "Divisors bit vector: " << divisors_sig.read() << endl;

        // Print success message
        cout << "All tests completed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
