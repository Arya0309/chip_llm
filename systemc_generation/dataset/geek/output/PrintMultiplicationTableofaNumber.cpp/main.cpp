
#include <systemc.h>

// Module to print multiplication table
SC_MODULE(MultiplicationTable) {
    // Input port for the number whose table is to be printed
    sc_in<int> number;
    
    // Process to print the multiplication table
    void printTable() {
        int n = number.read(); // Read the input number
        for (int i = 1; i <= 10; ++i) {
            // Print each line of the multiplication table
            cout << n << " * " << i << " = " << n * i << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(MultiplicationTable) {
        SC_METHOD(printTable);
        sensitive << number;
    }
};

// Testbench module to drive inputs
SC_MODULE(Testbench) {
    // Signal to connect with MultiplicationTable
    sc_signal<int> numSignal;

    // Instance of MultiplicationTable
    MultiplicationTable table;

    // Process to drive inputs
    void driveInput() {
        // Set the number for which the multiplication table is to be printed
        numSignal = 5;
        wait(1, SC_NS); // Wait for one delta cycle to let the table process
    }

    // Constructor to register the process and connect signals
    SC_CTOR(Testbench) : table("table") {
        // Connect the signal to the table's input port
        table.number(numSignal);

        // Registering the driveInput process
        SC_THREAD(driveInput);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
