
#include <systemc.h>

// Module to print the multiplication table
SC_MODULE(MultiplicationTable) {
    sc_in<int> n;  // Input port for the number whose table is to be printed

    // Constructor
    SC_CTOR(MultiplicationTable) {
        // Process to print the multiplication table
        SC_METHOD(print_table);
        sensitive << n;
    }

    // Method to print the multiplication table
    void print_table() {
        int num = n.read();
        for (int i = 1; i <= 10; ++i) {
            cout << num << " * " << i << " = " << num * i << endl;
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n_sig; // Signal for the number

    // Instance of the MultiplicationTable module
    MultiplicationTable mul_table_inst;

    // Constructor
    SC_CTOR(Testbench)
    : mul_table_inst("mul_table_inst") {
        // Connect signals to ports
        mul_table_inst.n(n_sig);

        // Process to set the value of n and trigger the table print
        SC_THREAD(set_value);
    }

    // Thread to set the value of n
    void set_value() {
        // Set the number to 5
        n_sig.write(5);
        wait(1, SC_NS); // Wait for the process to update

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
