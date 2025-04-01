
#include <systemc.h>

// Module to reverse an array and print both the original and reversed arrays
SC_MODULE(ArrayReverser) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> reset; // Reset signal

    // Constructor
    SC_CTOR(ArrayReverser) {
        // Process to reverse the array
        SC_METHOD(reverse_array);
        sensitive << clk.posedge_event();
        dont_initialize();

        // Process to print arrays
        SC_METHOD(print_arrays);
        sensitive << clk.posedge_event();
        dont_initialize();
    }

    void reverse_array() {
        if (!reset.read()) {
            static int original_arr[] = {1, 2, 3, 4, 5};
            static int copied_arr[5];
            static int len = sizeof(original_arr) / sizeof(original_arr[0]);
            static int i = 0;

            if (i < len) {
                copied_arr[i] = original_arr[len - i - 1];
                i++;
            }
        }
    }

    void print_arrays() {
        if (!reset.read() && i >= len) {
            static int original_arr[] = {1, 2, 3, 4, 5};
            static int copied_arr[5];
            static int len = sizeof(original_arr) / sizeof(original_arr[0]);

            // Print the original array
            cout << "Original array: ";
            for (int j = 0; j < len; j++) {
                cout << original_arr[j] << " ";
            }
            cout << endl;

            // Print the copied array
            cout << "Resultant array: ";
            for (int j = 0; j < len; j++) {
                cout << copied_arr[j] << " ";
            }
            cout << endl;

            sc_stop(); // Stop the simulation after printing
        }
    }

private:
    static const int len = 5; // Length of the array
    static int i; // Counter for reversing the array
};

int ArrayReverser::i = 0; // Initialize static variable

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk; // Clock signal
    sc_signal<bool> reset; // Reset signal

    ArrayReverser reverser_inst; // Instance of the ArrayReverser module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), reverser_inst("reverser") {
        // Connect signals to ports
        reverser_inst.clk(clk);
        reverser_inst.reset(reset);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initial reset
        reset.write(true);
        wait(2, SC_NS);
        reset.write(false);

        // Wait for the simulation to complete
        wait();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
