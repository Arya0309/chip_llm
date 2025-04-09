
#include <systemc.h>

// Module to reverse an array

SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal
    sc_out<bool> done;     // Done signal

    // Internal signals
    sc_signal<int> original_arr[5]; // Original array
    sc_signal<int> copied_arr[5];   // Copied array
    sc_signal<int> index;          // Index for array operations
    sc_signal<bool> processing;     // Processing flag

    // Constructor
    SC_CTOR(ArrayReverse) {
        // Initialize original array
        original_arr[0] = 1;
        original_arr[1] = 2;
        original_arr[2] = 3;
        original_arr[3] = 4;
        original_arr[4] = 5;

        // Process to handle array reversal
        SC_THREAD(reverse_array);
        sensitive << clk.pos();
        async_reset_signal_is(start, false);

        // Process to print the arrays
        SC_THREAD(print_arrays);
        sensitive << clk.pos();
        async_reset_signal_is(start, false);
    }

    // Method to reverse the array
    void reverse_array() {
        index = 0;
        processing = true;
        while (index < 5) {
            wait(); // Wait for clock edge
            copied_arr[index].write(original_arr[4 - index].read());
            index++;
        }
        processing = false;
        done.write(true);
    }

    // Method to print the arrays
    void print_arrays() {
        wait(); // Wait for start signal
        cout << "Original array: ";
        for (int i = 0; i < 5; i++) {
            cout << original_arr[i].read() << " ";
        }
        cout << endl;

        wait(); // Wait for processing to complete
        cout << "Resultant array: ";
        for (int i = 0; i < 5; i++) {
            cout << copied_arr[i].read() << " ";
        }
        cout << endl;
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
