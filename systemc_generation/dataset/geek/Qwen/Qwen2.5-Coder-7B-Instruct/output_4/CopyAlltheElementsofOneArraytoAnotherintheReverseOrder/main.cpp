
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> start;  // Start signal
    sc_out<bool> done;  // Done signal
    sc_out<int> original_arr[5];  // Output array for original values
    sc_out<int> copied_arr[5];  // Output array for reversed values

    SC_CTOR(ArrayReverse) {
        SC_METHOD(reverse_array);
        sensitive << clk.pos();
        dont_initialize();
    }

    void reverse_array() {
        static bool started = false;
        static int index = 0;

        if (start.read()) {
            started = true;
            index = 0;
        }

        if (started && !done.read() && index < 5) {
            copied_arr[index].write(original_arr[4 - index].read());
            index++;
        }

        if (index >= 5) {
            done.write(true);
        }

        if (!start.read()) {
            started = false;
            done.write(false);
        }
    }

    void load_array(const int arr[5]) {
        for (int i = 0; i < 5; i++) {
            original_arr[i].write(arr[i]);
        }
    }

    void read_array(int result[5]) {
        for (int i = 0; i < 5; i++) {
            result[i] = copied_arr[i].read();
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
