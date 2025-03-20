
#include <systemc.h>

// Module to reverse the array and print both original and reversed arrays
SC_MODULE(ArrayReverser) {
    sc_in<bool> clk; // Clock signal to synchronize operations
    sc_in<bool> rst; // Reset signal to initialize the system

    // Constructor
    SC_CTOR(ArrayReverser) {
        // Register the method to reverse the array
        SC_METHOD(reverse_array);
        sensitive << clk.posedge_event() << rst;

        // Register the method to print the arrays
        SC_METHOD(print_arrays);
        sensitive << clk.posedge_event() << rst;
    }

    // Method to reverse the array
    void reverse_array() {
        if (rst.read()) {
            // Reset logic
            for (int i = 0; i < len; ++i) {
                original_arr[i] = 0;
                copied_arr[i] = 0;
            }
            step = 0;
        } else {
            if (step < len) {
                copied_arr[step] = original_arr[len - step - 1];
                step++;
            }
        }
    }

    // Method to print the arrays
    void print_arrays() {
        if (rst.read()) {
            // Do nothing during reset
        } else {
            if (step == len) {
                sc_assert(step == len);
                cout << "Original array: ";
                for (int i = 0; i < len; ++i) {
                    cout << original_arr[i] << " ";
                }
                cout << endl;

                cout << "Resultant array: ";
                for (int i = 0; i < len; ++i) {
                    cout << copied_arr[i] << " ";
                }
                cout << endl;

                // Stop the simulation after printing
                sc_stop();
            }
        }
    }

private:
    static const int len = 5; // Length of the array
    int original_arr[len] = {1, 2, 3, 4, 5}; // Original array
    int copied_arr[len]; // Array to hold the reversed elements
    int step; // Step counter to track progress
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk; // Clock signal
    sc_signal<bool> rst; // Reset signal

    ArrayReverser reverser_inst; // Instance of the ArrayReverser module

    // Constructor
    SC_CTOR(Testbench) : reverser_inst("reverser") {
        // Connect signals to ports
        reverser_inst.clk(clk);
        reverser_inst.rst(rst);

        // Create a clock generator
        SC_THREAD(clock_gen);
        // Create a reset signal generator
        SC_THREAD(reset_gen);
    }

    // Clock generator thread
    void clock_gen() {
        clk = 0;
        while (true) {
            wait(1, SC_NS); // Toggle every 1 ns
            clk = !clk;
        }
    }

    // Reset generator thread
    void reset_gen() {
        rst = 1; // Assert reset
        wait(2, SC_NS);
        rst = 0; // Deassert reset
        wait(SC_ZERO_TIME); // Wait for one delta cycle
        sc_start(); // Continue simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
