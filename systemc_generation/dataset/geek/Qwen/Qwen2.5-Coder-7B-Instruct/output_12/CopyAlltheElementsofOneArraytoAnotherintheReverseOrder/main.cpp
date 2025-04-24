
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;       // Clock signal
    sc_in<bool> start;     // Start signal
    sc_out<bool> done;    // Done signal

    // Array lengths
    const int len = 5;
    
    // Input array
    int original_arr[len] = {1, 2, 3, 4, 5};
    
    // Output array
    int copied_arr[len];
    
    // Counter for array elements
    sc_signal<int> i;
    
    // Method to initialize the arrays and copy in reverse order
    void copyReverse() {
        for (i = 0; i < len; i++) {
            copied_arr[i] = original_arr[len - i - 1];
        }
    }
    
    // Method to print the array (simulated by printing to cout)
    void printArray(int arr[], int len) {
        for (int j = 0; j < len; j++) {
            cout << arr[j] << " ";
        }
        cout << endl;
    }
    
    // Constructor
    SC_CTOR(ArrayReverse) {
        // Process to handle the start signal and execute the reversal
        SC_METHOD(handleStart);
        sensitive << start.pos();
        
        // Process to update the done signal when reversal is complete
        SC_METHOD(updateDone);
        sensitive << clk.pos();
        dont_initialize();
    }
    
    // Method to handle the start signal
    void handleStart() {
        if (start.read()) {
            copyReverse();
            cout << "Original array: ";
            printArray(original_arr, len);
            cout << "Resultant array: ";
            printArray(copied_arr, len);
            done.write(true);
        }
    }
    
    // Method to update the done signal
    void updateDone() {
        done.write(false);
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
