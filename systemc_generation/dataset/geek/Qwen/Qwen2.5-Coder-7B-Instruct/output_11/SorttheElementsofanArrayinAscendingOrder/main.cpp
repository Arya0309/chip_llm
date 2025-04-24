
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;     // Clock signal
    sc_in<bool> start;   // Start signal
    sc_out<bool> done;  // Done signal
    sc_inout<int> nums;  // Array to be sorted
    sc_in<int> size_nums; // Size of the array

    bool isSwapped;

    SC_CTOR(BubbleSort) {
        SC_METHOD(sort);
        sensitive << clk.pos();
        async_reset_signal_is(clk, false);
    }

    void sort() {
        if (start.read()) {
            for (int i = 0; i < size_nums.read(); i++) {
                isSwapped = false;
                for (int j = 1; j < size_nums.read() - i; j++) {
                    if (nums.read()[j] < nums.read()[j - 1]) {
                        swapNums(nums, j, j - 1);
                        isSwapped = true;
                    }
                }
                if (!isSwapped) {
                    break;
                }
            }
            done.write(true);
        } else {
            done.write(false);
        }
    }

    void swapNums(sc_inout<int>& nums, int first, int second) {
        int curr = nums.read()[first];
        nums.write(first, nums.read()[second]);
        nums.write(second, curr);
    }
};


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
