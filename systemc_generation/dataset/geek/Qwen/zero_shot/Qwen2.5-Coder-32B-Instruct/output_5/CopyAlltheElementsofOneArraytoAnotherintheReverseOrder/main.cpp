
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    SC_CTOR(ArrayReverse) {
        SC_METHOD(process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void process() {
        static bool processing = false;
        static int step = 0;
        static const int original_arr[5] = {1, 2, 3, 4, 5};
        static int copied_arr[5];
        static const int len = sizeof(original_arr) / sizeof(original_arr[0]);

        if (start.read()) {
            processing = true;
            step = 0;
        }

        if (processing) {
            if (step < len) {
                copied_arr[step] = original_arr[len - step - 1];
                step++;
            } else {
                // Print arrays (this would typically be done outside the module in a testbench)
                std::cout << "Original array: ";
                for (int i = 0; i < len; i++) {
                    std::cout << original_arr[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "Resultant array: ";
                for (int i = 0; i < len; i++) {
                    std::cout << copied_arr[i] << " ";
                }
                std::cout << std::endl;

                done.write(true);
                processing = false;
            }
        } else {
            done.write(false);
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
