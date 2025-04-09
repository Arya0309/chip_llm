
#include <systemc.h>

// Define a constant for the size of the array
static const int ARRAY_SIZE = 5;

// SystemC module to reverse an array

SC_MODULE(ArrayReverse) {
    // Clock signal
    sc_in<bool> clk;
    // Start signal to initiate the process
    sc_in<bool> start;
    // Done signal to indicate completion
    sc_out<bool> done;
    // Input array
    sc_in<int> original_arr[ARRAY_SIZE];
    // Output array
    sc_out<int> copied_arr[ARRAY_SIZE];

    // Internal storage for the original array
    int original_storage[ARRAY_SIZE];
    // Internal storage for the copied array
    int copied_storage[ARRAY_SIZE];

    SC_CTOR(ArrayReverse) {
        // Initialize done to false
        done.write(false);

        // Register the process that reverses the array
        SC_METHOD(reverse_array);
        sensitive << clk.pos();
        dont_initialize();
    }

    // Method to reverse the array
    void reverse_array() {
        if (start.read()) {
            // Read the original array
            for (int i = 0; i < ARRAY_SIZE; i++) {
                original_storage[i] = original_arr[i].read();
            }

            // Reverse the array and store in copied_storage
            for (int i = 0; i < ARRAY_SIZE; i++) {
                copied_storage[i] = original_storage[ARRAY_SIZE - i - 1];
            }

            // Write the reversed array to copied_arr
            for (int i = 0; i < ARRAY_SIZE; i++) {
                copied_arr[i].write(copied_storage[i]);
            }

            // Signal completion
            done.write(true);
        } else {
            // Reset done signal if not started
            done.write(false);
        }
    }
};

// Example usage in a testbench


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
