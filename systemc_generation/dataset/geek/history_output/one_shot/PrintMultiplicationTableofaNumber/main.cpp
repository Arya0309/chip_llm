
#include <systemc.h>

// Module to print multiplication table
SC_MODULE(MultiplicationTable) {
    sc_in<int> number;  // Input port for the number
    sc_out<sc_bv<32>> result; // Output port for the result

    // Constructor
    SC_CTOR(MultiplicationTable) {
        // Process to generate multiplication table
        SC_METHOD(generate_table);
        sensitive << number;
    }

    // Method to generate the multiplication table
    void generate_table() {
        int n = number.read();
        for (int i = 1; i <= 10; ++i) {
            int res = n * i;
            cout << n << " * " << i << " = " << res << endl;
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> number; // Signal for the number
    sc_bv<32> result; // Result signal (not used in this simple example)

    MultiplicationTable mul_inst; // Instance of the MultiplicationTable module

    // Constructor
    SC_CTOR(Testbench) : mul_inst("mul_table") {
        // Connect signals to ports
        mul_inst.number(number);
        mul_inst.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the number for which the multiplication table is to be printed
        number = 5;
        wait(1, SC_NS); // Wait for the multiplication table to be printed

        // Print completion message
        cout << "Multiplication table printed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
