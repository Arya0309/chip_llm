
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal to begin processing
    sc_out<bool> done;     // Done signal indicating completion

    // Internal signals and variables
    sc_signal<sc_uint<3>> index; // Index for iterating through arrays
    sc_signal<sc_uint<3>> len;   // Length of the array
    sc_signal<int> original_arr[5]; // Original array
    sc_signal<int> copied_arr[5];   // Copied array in reverse order

    // Constructor
    SC_CTOR(ArrayReverse) {
        // Initialize the original array with values
        original_arr[0].write(1);
        original_arr[1].write(2);
        original_arr[2].write(3);
        original_arr[3].write(4);
        original_arr[4].write(5);

        // Set the length of the array
        len.write(5);

        // Process to handle the array reversal
        SC_METHOD(process);
        sensitive << clk.pos(); // Sensitive to positive edge of clock
    }

    void process() {
        if (start.read()) { // Check if start signal is active
            if (index.read() < len.read()) { // If index is less than array length
                // Copy elements in reverse order
                copied_arr[index.read()].write(original_arr[len.read() - index.read() - 1].read());
                index.write(index.read() + 1); // Increment index
            } else {
                // If all elements are copied, set done signal
                done.write(true);
            }
        } else {
            // Reset index and done signal when start is not active
            index.write(0);
            done.write(false);
        }
    }
};

// Testbench module to simulate the ArrayReverse module


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
