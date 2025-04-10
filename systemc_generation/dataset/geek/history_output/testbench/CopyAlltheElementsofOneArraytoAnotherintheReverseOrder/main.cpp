#include <systemc.h>

// ArrayReverse Module: Reverses a fixed-size array of 5 elements.
SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Start signal to trigger reversal
    sc_out<bool> done;    // Signal to indicate that reversal is complete

    // Internal storage for the arrays
    int original_arr[5];
    int reversed_arr[5];

    // Constructor: Register the process with sensitivity to the positive edge of the clock.
    SC_CTOR(ArrayReverse) {
        SC_THREAD(reverse_process);
        sensitive << clk.pos();
    }

    // Process to reverse the array when start signal is asserted.
    void reverse_process() {
        while (true) {
            wait(); // Wait for the next clock cycle.
            if (start.read() == true) {
                // Reverse the array: copied element by element.
                for (int i = 0; i < 5; i++) {
                    reversed_arr[i] = original_arr[5 - i - 1];
                }
                done.write(true); // Signal that reversal is done.
                wait();           // Wait for one clock cycle.
                done.write(false);
            }
        }
    }

    // Helper method to load an input array into the module.
    void load_array(const int input[5]) {
        for (int i = 0; i < 5; i++) {
            original_arr[i] = input[i];
        }
    }

    // Helper method to read the reversed array from the module.
    void read_array(int output[5]) {
        for (int i = 0; i < 5; i++) {
            output[i] = reversed_arr[i];
        }
    }
};

// Testbench Module: Loads the array, triggers the reversal, and verifies the result.
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
