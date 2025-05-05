
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> start;  // Start signal
    sc_out<bool> done;  // Done signal

    sc_signal<int> original_arr[5];  // Signal to hold the original array
    sc_signal<int> copied_arr[5];    // Signal to hold the copied array
    sc_signal<bool> print_original; // Signal to trigger printing original array
    sc_signal<bool> print_copied;   // Signal to trigger printing copied array

    // Constructor
    SC_CTOR(ArrayReverse) {
        // Process to handle start signal and reverse array
        SC_METHOD(handle_start);
        sensitive << start;

        // Process to print arrays
        SC_METHOD(print_arrays);
        sensitive << print_original << print_copied << clk.pos();
    }

    // Method to handle start signal and reverse array
    void handle_start() {
        if (start.read()) {
            // Initialize original array
            original_arr[0].write(1);
            original_arr[1].write(2);
            original_arr[2].write(3);
            original_arr[3].write(4);
            original_arr[4].write(5);

            // Copy and reverse the array
            for (int i = 0; i < 5; i++) {
                copied_arr[i].write(original_arr[4 - i].read());
            }

            // Trigger printing
            print_original.write(true);
            print_copied.write(true);
        }
    }

    // Method to print arrays
    void print_arrays() {
        static bool printed = false;
        if (print_original.read() && !printed) {
            cout << " Original array: ";
            for (int i = 0; i < 5; i++) {
                cout << original_arr[i].read() << " ";
            }
            cout << endl;
            print_original.write(false);
            printed = true;
        } else if (print_copied.read() && !printed) {
            cout << " Resultant array: ";
            for (int i = 0; i < 5; i++) {
                cout << copied_arr[i].read() << " ";
            }
            cout << endl;
            print_copied.write(false);
            printed = true;
        } else if (printed) {
            done.write(true);
        }
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
