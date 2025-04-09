
#include <systemc.h>


SC_MODULE(ArrayReverse) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal to begin processing
    sc_out<bool> done;    // Done signal to indicate completion

    // Internal signals and variables
    sc_signal<int> original_arr[5];
    sc_signal<int> copied_arr[5];
    sc_signal<int> len;
    sc_signal<int> i;

    // Constructor
    SC_CTOR(ArrayReverse) {
        // Process to handle the reversal operation
        SC_THREAD(reverse_array);
        sensitive << clk.pos();
    }

    void reverse_array() {
        wait(); // Wait for the first clock edge
        if (start.read()) {
            // Initialize original array
            original_arr[0].write(1);
            original_arr[1].write(2);
            original_arr[2].write(3);
            original_arr[3].write(4);
            original_arr[4].write(5);

            len.write(5); // Set the length of the array
            i.write(0);   // Initialize index

            // Wait for the next clock edge
            while (i.read() < len.read()) {
                wait(); // Wait for the next clock edge
                copied_arr[i.read()].write(original_arr[len.read() - i.read() - 1].read());
                i.write(i.read() + 1);
            }

            // Signal completion
            done.write(true);
        } else {
            done.write(false);
        }
    }
};

// Example testbench module

SC_MODULE(TopLevel) {
    sc_clock clk;
    sc_signal<bool> start;
    sc_signal<bool> done;

    ArrayReverse reverser;
    Testbench tb;

    SC_CTOR(TopLevel) : clk("clk", 1, SC_NS), reverser("reverser"), tb("tb") {
        reverser.clk(clk);
        reverser.start(start);
        reverser.done(done);

        tb.clk(clk);
        tb.start(start);
        tb.done(done);
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
