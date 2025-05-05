
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> start;  // Start signal
    sc_out<bool> done;  // Done signal

    static const int len = 5;
    int* original_arr;
    int* copied_arr;
    bool is_copied = false;

    SC_CTOR(ArrayReverse) {
        original_arr = new int[len];
        copied_arr = new int[len];
        for (int i = 0; i < len; i++) {
            original_arr[i] = i + 1;
        }
        SC_METHOD(copy_and_print);
        sensitive << clk.pos();
        dont_initialize();
    }

    ~ArrayReverse() {
        delete[] original_arr;
        delete[] copied_arr;
    }

    void copy_and_print() {
        if (start.read()) {
            for (int i = 0; i < len; i++) {
                copied_arr[i] = original_arr[len - i - 1];
            }
            is_copied = true;
        }

        if (is_copied) {
            std::cout << " Original array: ";
            for (int i = 0; i < len; i++) {
                std::cout << original_arr[i] << " ";
            }
            std::cout << std::endl;

            std::cout << " Resultant array: ";
            for (int i = 0; i < len; i++) {
                std::cout << copied_arr[i] << " ";
            }
            std::cout << std::endl;

            is_copied = false;
            done.write(true);
        } else {
            done.write(false);
        }
    }

    void load_array(const int* arr) {
        for (int i = 0; i < len; i++) {
            original_arr[i] = arr[i];
        }
    }

    void read_array(int* arr) {
        for (int i = 0; i < len; i++) {
            arr[i] = copied_arr[i];
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
