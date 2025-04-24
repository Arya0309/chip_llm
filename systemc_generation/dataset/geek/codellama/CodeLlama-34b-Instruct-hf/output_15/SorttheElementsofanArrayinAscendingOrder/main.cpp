c++
#include <systemc.h>

// Module that sorts an array of integers using the bubble sort algorithm

SC_MODULE(BubbleSort) {
    sc_in<int> nums[5]; // Input port for the array to be sorted
    sc_in<int> len; // Input port for the length of the array
    sc_out<int> sorted_nums[5]; // Output port for the sorted array

    // Miscellaneous ports
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Constructor
    SC_CTOR(BubbleSort) {
        // Process to sort the input array
        SC_THREAD(sort);
        sensitive << clk.pos(); // Clock signal
        do_initialize(); // Initialize the module
    }

    // Method to sort the input array
    void sort() {
        int nums_cpy[5]; // Temporary array to store a copy of the input array
        int curr; // Current element
        int next; // Next element
        bool isSwapped; // Flag to indicate whether any swaps occurred in this iteration

        // Initialize the temporary array with the input array
        for (int i = 0; i < 5; i++) {
            nums_cpy[i] = nums[i].read();
        }

        // Iterate through the array, starting from the first index and ending at the last index minus the current iteration index
        for (int i = 0; i < len.read(); i++) {
            isSwapped = false;
            for (int j = 1; j < len.read() - i; j++) {
                // Check if the current element is smaller than the previous element
                if (nums_cpy[j] < nums_cpy[j - 1]) {
                    // Swap the two elements
                    curr = nums_cpy[j];
                    next = nums_cpy[j - 1];
                    nums_cpy[j] = next;
                    nums_cpy[j - 1] = curr;
                    isSwapped = true;
                }
            }
            // Check if any swaps occurred in this iteration
            if (!isSwapped) {
                // No swaps occurred, so the array is already sorted
                break;
            }
        }

        // Write the sorted array to the output port
        for (int i = 0; i < 5; i++) {
            sorted_nums[i].write(nums_cpy[i]);
        }
    }

    // Method to initialize the module
    void do_initialize() {
        // Reset the done flag
        done.write(false);
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
