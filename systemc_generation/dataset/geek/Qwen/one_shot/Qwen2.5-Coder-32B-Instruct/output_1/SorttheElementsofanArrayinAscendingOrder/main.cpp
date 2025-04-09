
#include <systemc.h>

// Define constants
static const int SIZE = 5;

// Module that sorts an array using bubble sort

SC_MODULE(BubbleSort) {
    // Input signals
    sc_in<bool> clk;
    sc_in<bool> start;
    // Output signal
    sc_out<bool> done;

    // Internal storage for the array
    sc_signal<int> nums[SIZE];
    // Internal variables
    sc_signal<int> i, j;
    sc_signal<bool> isSwapped, running;

    SC_CTOR(BubbleSort) {
        // Initialize the array with default values
        nums[0] = 1;
        nums[1] = 12;
        nums[2] = 6;
        nums[3] = 8;
        nums[4] = 10;

        // Process to handle sorting
        SC_CTHREAD(sort_process, clk.pos());
        reset_signal_is(start, false);
    }

    void sort_process() {
        while (true) {
            wait(); // Wait for the clock edge
            if (start.read()) { // Check if sorting should start
                running.write(true);
                done.write(false);
                i.write(0);
                while (i.read() < SIZE && running.read()) {
                    isSwapped.write(false);
                    j.write(1);
                    while (j.read() < SIZE - i.read() && running.read()) {
                        if (nums[j.read()].read() < nums[j.read() - 1].read()) {
                            // Swap the elements
                            int temp = nums[j.read()].read();
                            nums[j.read()].write(nums[j.read() - 1].read());
                            nums[j.read() - 1].write(temp);
                            isSwapped.write(true);
                        }
                        j.write(j.read() + 1);
                    }
                    if (!isSwapped.read()) {
                        running.write(false);
                    }
                    i.write(i.read() + 1);
                }
                done.write(true);
            }
        }
    }
};

// Example usage


SC_MODULE(Testbench) {
    sc_clock clk;          // Clock signal for synchronization
    sc_signal<bool> start; // Signal to trigger the sort
    sc_signal<bool> done;  // Signal indicating sort completion

    // Instance of the BubbleSort module
    BubbleSort* bubble_sort_inst;

    SC_CTOR(Testbench) : clk("clk", 1, SC_NS) {
        bubble_sort_inst = new BubbleSort("bubble_sort_inst");
        bubble_sort_inst->clk(clk);
        bubble_sort_inst->start(start);
        bubble_sort_inst->done(done);

        SC_THREAD(run_tests);
    }

    // Process to run the test cases
    void run_tests() {
        // Initialize the array with unsorted values {1, 12, 6, 8, 10}
        int arr_in[5] = {1, 12, 6, 8, 10};
        bubble_sort_inst->load_array(arr_in);

        // Print the array before sorting
        cout << "Before sorting: ";
        for (int i = 0; i < 5; i++) {
            cout << arr_in[i] << " ";
        }
        cout << endl;

        // Start the sorting process
        start.write(true);
        wait(1, SC_NS); // wait for one clock cycle
        start.write(false);

        // Wait until the BubbleSort module signals that sorting is complete
        while (done.read() != true) {
            wait(1, SC_NS);
        }
        
        // Retrieve the sorted array from the BubbleSort module
        int arr_out[5];
        bubble_sort_inst->read_array(arr_out);

        // Print the sorted array
        cout << "After sorting: ";
        for (int i = 0; i < 5; i++) {
            cout << arr_out[i] << " ";
        }
        cout << endl;

        // Expected sorted result is {1, 6, 8, 10, 12}
        int expected[5] = {1, 6, 8, 10, 12};
        for (int i = 0; i < 5; i++) {
            assert(arr_out[i] == expected[i]);
            cout << "Element " << i << " sorted value: " << arr_out[i] << endl;
        }
        cout << "All tests passed successfully." << endl;

        sc_stop(); // End simulation
    }
};

// sc_main: Entry point for the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create the testbench instance
    sc_start();         // Start the simulation
    return 0;
}
