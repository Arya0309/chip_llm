
#include <systemc.h>

// Module to print divisors of a number
SC_MODULE(DivisorPrinter) {
    sc_in<int> n;  // Input port for the number
    sc_out<sc_bv<32>> divisors; // Output port for divisors (as a bit vector)

    // Constructor
    SC_CTOR(DivisorPrinter) {
        // Process to print divisors
        SC_METHOD(print_divisors);
        sensitive << n;
    }

    // Method to find and print divisors
    void print_divisors() {
        int num = n.read();
        sc_bv<32> divs = 0; // Bit vector to store divisors
        int count = 0;

        for (int i = 1; i <= num; i++) {
            if (num % i == 0) {
                cout << " " << i;
                divs[count++] = 1; // Mark the position as a divisor
            }
        }
        divisors.write(divs); // Write the divisors to the output port
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n; // Signal for the number
    sc_signal<sc_bv<32>> divisors; // Signal for the divisors

    DivisorPrinter divisor_printer_inst; // Instance of the DivisorPrinter module

    // Constructor
    SC_CTOR(Testbench) : divisor_printer_inst("divisor_printer") {
        // Connect signals to ports
        divisor_printer_inst.n(n);
        divisor_printer_inst.divisors(divisors);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the number to 100
        n = 100;
        wait(1, SC_NS); // Wait for the divisor printer to process
        cout << "\nThe divisors of " << n.read() << " are:" << endl;

        // Print the divisors
        sc_bv<32> divs = divisors.read();
        for (int i = 0; i < 32; i++) {
            if (divs[i] == 1) {
                cout << " " << i + 1;
            }
        }
        cout << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
