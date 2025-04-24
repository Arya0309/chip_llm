scss
#include <systemc.h>

// Module that copies an array in reverse order

SC_MODULE(ArrayReverse) {
    sc_in<int> clk;    // Clock
    sc_in<bool> start;  // Start signal
    sc_out<bool> done;  // Done signal

    sc_in<int> original_arr[5];  // Input array
    sc_in<int> copied_arr[5];    // Output array
    sc_in<int> len;              // Length of the arrays

    // Constructor
    SC_CTOR(ArrayReverse) :
        clk("clk"), start("start"), done("done"),
        original_arr(*this), copied_arr(*this), len(*this) {}

    // Process to perform the copy operation
    void copy() {
        sc_int<32> i = 0;
        while (i < len) {
            copied_arr[i] = original_arr[len - i - 1];
            i++;
        }
    }

    // Generate code for the process
    void generate_code() {
        // Set the clock period to 1ns and enable it
        clock.set_period(1);
        clock.enable();

        // Create a process to copy the array
        sc_process process = sc_spawn(copy);

        // Wait for the process to finish
        sc_future<bool> finished;
        wait(finished);
        // Finished signal, set by the process
        sc_out<bool> finished("finished");

        // Generate the code
        generate_code_with_controls({finished});
    }
};

// Driver code
int main() {
    sc_int<32> len = 5;
    int original_arr[5] = {1, 2, 3, 4, 5};
    int copied_arr[5];
    sc_int<1> clk;
    sc_int<1> start;
    sc_int<1> done;

    ArrayReverse reverse_array(clk, start, done, len, original_arr, copied_arr);

    // Initialize the clock signal
    clk = 0;

    // Set the start signal high
    start = 1;

    // Wait for the done signal to be asserted
    sc_future<bool> done;
    wait(done);
    // Done signal, set by the ArrayReverse module
    sc_out<bool> done("done");

    // Success
    return 0;
}


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
