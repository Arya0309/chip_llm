
#include <systemc.h>

// ArrayReverse Module: Copies an array into another in reverse order

SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Signal to start copying
    sc_out<bool> done;    // Signal to indicate copying is complete

    // Internal storage for the arrays (fixed size of 5 elements)
    int original_arr[5];
    int copied_arr[5];

    // Constructor: register the process
    SC_CTOR(ArrayReverse) {
        SC_THREAD(copy_process);
        sensitive << clk.pos();
    }

    // Process that waits for the start signal and performs copying
    void copy_process() {
        while (true) {
            wait(); // wait for a clock cycle
            if (start.read() == true) {
                copyArrayInReverse(); // perform copying
                done.write(true);
                wait(); // wait for one cycle to signal completion
                done.write(false);
            }
        }
    }

    // Function to copy the array in reverse order
    void copyArrayInReverse() {
        const int len = 5;
        for (int i = 0; i < len; i++) {
            copied_arr[i] = original_arr[len - i - 1];
        }
    }

    // Helper method to load an input array into the module
    void load_array(const int input[5]) {
        for (int i = 0; i < 5; i++) {
            original_arr[i] = input[i];
        }
    }

    // Helper method to read the copied array from the module
    void read_array(int output[5]) {
        for (int i = 0; i < 5; i++) {
            output[i] = copied_arr[i];
        }
    }
};


SC_MODULE(Testbench) {
    sc_clock clk;          // Clock signal for synchronization
    sc_signal<bool> start; // Signal to trigger the reversal
    sc_signal<bool> done;  // Signal indicating the reversal is complete

    // Instance of the ArrayReverse module
    ArrayReverse* array_rev_inst;

    SC_CTOR(Testbench) : clk("clk", 1, SC_NS) {
        array_rev_inst = new ArrayReverse("array_rev_inst");
        array_rev_inst->clk(clk);
        array_rev_inst->start(start);
        array_rev_inst->done(done);

        SC_THREAD(run_tests);
    }

    // Process to run the test cases.
    void run_tests() {
        // Initialize the original array with fixed values {1, 2, 3, 4, 5}
        int original[5] = {1, 2, 3, 4, 5};
        // Expected reversed result is {5, 4, 3, 2, 1}
        int expected[5] = {5, 4, 3, 2, 1};

        // Load the original array into the ArrayReverse module.
        array_rev_inst->load_array(original);

        // Trigger the reversal process.
        start.write(true);
        wait(1, SC_NS); // Wait for one clock cycle.
        start.write(false);

        // Wait until the module signals that the reversal is done.
        while (done.read() != true) {
            wait(1, SC_NS);
        }
        
        // Retrieve the reversed array.
        int result[5];
        array_rev_inst->read_array(result);

        // Print the original and reversed arrays.
        cout << "Original Array: ";
        for (int i = 0; i < 5; i++) {
            cout << original[i] << " ";
        }
        cout << "\nReversed Array: ";
        for (int i = 0; i < 5; i++) {
            cout << result[i] << " ";
        }
        cout << endl;

        // Verify that the reversed array matches the expected result.
        for (int i = 0; i < 5; i++) {
            assert(result[i] == expected[i]);
        }
        cout << "All tests passed successfully." << endl;

        sc_stop(); // End the simulation.
    }
};

// sc_main: Entry point for the SystemC simulation.
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create the testbench instance.
    sc_start();         // Start the simulation.
    return 0;
}
