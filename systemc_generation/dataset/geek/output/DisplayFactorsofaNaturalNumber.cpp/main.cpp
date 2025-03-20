
#include <systemc.h>

// SystemC module to find and print divisors of a number
SC_MODULE(DivisorPrinter) {
    // Input port for the number to find divisors for
    sc_in<int> number_in;

    // Process to find and print divisors
    void printDivisors() {
        int n = number_in.read(); // Read the input number
        cout << "The divisors of " << n << " are: ";
        for (int i = 1; i <= n; i++) {
            if (n % i == 0) { // Check if i is a divisor
                cout << " " << i; // Print the divisor
            }
        }
        cout << endl; // Newline after printing all divisors
    }

    // Constructor to register the process
    SC_CTOR(DivisorPrinter) {
        SC_METHOD(printDivisors);
        sensitive << number_in; // Sensitive to changes in input number
    }
};

// Testbench module to drive inputs to DivisorPrinter
SC_MODULE(Testbench) {
    // Signal to connect with DivisorPrinter
    sc_signal<int> number_sig;

    // Instance of DivisorPrinter
    DivisorPrinter divisor_printer;

    // Process to drive inputs
    void driveInputs() {
        number_sig = 100; // Set the number to 100
        wait(1, SC_NS);   // Wait for one delta cycle
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : divisor_printer("divisor_printer") {
        // Connecting signals to the DivisorPrinter port
        divisor_printer.number_in(number_sig);

        // Registering the driveInputs process
        SC_THREAD(driveInputs);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
