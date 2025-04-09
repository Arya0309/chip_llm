
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal
    sc_out<bool> done;    // Done signal

    // Internal signals and variables
    sc_signal<sc_uint<3>> counter;  // Counter to iterate over the array
    sc_signal<sc_uint<3>> len;      // Length of the array
    sc_signal<sc_uint<8>> original_arr[5]; // Original array
    sc_signal<sc_uint<8>> copied_arr[5];   // Copied array in reverse order

    // Constructor
    SC_CTOR(ArrayReverse) {
        // Initialize the length of the array
        len = 5;

        // Initialize the original array
        original_arr[0] = 1;
        original_arr[1] = 2;
        original_arr[2] = 3;
        original_arr[3] = 4;
        original_arr[4] = 5;

        // Reset the done signal
        done.write(false);

        // Process to handle the start signal and reverse the array
        SC_METHOD(reverse_array);
        sensitive << clk.pos();
    }

    // Method to reverse the array
    void reverse_array() {
        if (start.read()) {
            // Initialize counter
            counter = 0;

            // Loop to copy elements in reverse order
            while (counter < len.read()) {
                copied_arr[counter] = original_arr[len.read() - counter - 1];
                counter = counter + 1;
            }

            // Signal completion
            done.write(true);
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
