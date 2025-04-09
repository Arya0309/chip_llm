
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;       // Clock signal
    sc_in<bool> start;     // Start signal
    sc_out<bool> done;      // Done signal
    sc_out<int> resultant_array[5]; // Output array to hold the reversed elements

    int original_arr[5] = {1, 2, 3, 4, 5}; // Original array
    int len = sizeof(original_arr) / sizeof(original_arr[0]); // Length of the array
    bool processing = false; // Flag to indicate if processing is ongoing

    // Constructor
    SC_CTOR(ArrayReverse) {
        // Process to handle start and clock signals
        SC_THREAD(run);
        sensitive << clk.pos();
    }

    // Thread to handle the main logic
    void run() {
        while (true) {
            wait(); // Wait for the next clock cycle

            if (start.read()) {
                processing = true;
                done.write(false);

                // Copy the elements of the array in the copied_arr in Reverse Order
                for (int i = 0; i < len; i++) {
                    resultant_array[i].write(original_arr[len - i - 1]);
                }

                processing = false;
                done.write(true);
            }
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
