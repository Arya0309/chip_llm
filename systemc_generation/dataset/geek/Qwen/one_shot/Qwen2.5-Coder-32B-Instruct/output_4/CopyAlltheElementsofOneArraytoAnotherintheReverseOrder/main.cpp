
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;       // Clock signal
    sc_in<bool> start;     // Start signal
    sc_out<bool> done;      // Done signal

    // Internal signals and variables
    sc_signal<bool> processing;
    int original_arr[5];
    int copied_arr[5];
    int len;
    int i;

    // Constructor
    SC_CTOR(ArrayReverse) {
        len = 5; // Length of the array
        for (i = 0; i < len; i++) {
            original_arr[i] = i + 1; // Initialize original array
        }
        i = 0;
        processing = false;
        done.write(false);

        // Sensitivity list for the process
        SC_METHOD(reverseArray);
        sensitive << clk.pos();
    }

    // Method to reverse the array
    void reverseArray() {
        if (start.read() && !processing.read()) {
            processing = true;
        }

        if (processing.read()) {
            if (i < len) {
                copied_arr[i] = original_arr[len - i - 1]; // Copy in reverse order
                i++;
            } else {
                processing = false;
                done.write(true);
                printArrays(); // Print the arrays after copying
            }
        }
    }

    // Function to print the arrays
    void printArrays() {
        cout << "Original array: ";
        for (int j = 0; j < len; j++) {
            cout << original_arr[j] << " ";
        }
        cout << endl;

        cout << "Resultant array: ";
        for (int j = 0; j < len; j++) {
            cout << copied_arr[j] << " ";
        }
        cout << endl;
    }
};

// Testbench module


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
