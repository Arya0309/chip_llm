
#include <systemc.h>

// Module that performs bubble sort on an integer array

SC_MODULE(BubbleSort) {
    // Input ports
    sc_in<int> num[32]; // Integer array to be sorted
    sc_in<int> len;    // Length of the array

    // Output port
    sc_out<int> sorted_num[32]; // BubbleSorted integer array

    // Constructor
    SC_CTOR(BubbleSort) {
        // Configures the input and output ports
        SC_CTHREAD(sort_num, clk.pos());
        reset_signal_is(reset, true);
    }

    void sort_num() {
        bool is_sorted = false; // Flag to indicate if the array is sorted

        while (!is_sorted) {
            // Iterates over the array
            for (int i = 1; i < len; i++) {
                // Checks if the current element is smaller than the previous one
                if (num[i] < num[i - 1]) {
                    // Swaps the elements if they are in the wrong order
                    swap_num(i, i - 1);
                }
            }

            if (is_sorted) {
                // Breaks out of the inner loop if the array is sorted
                break;
            }
        }

        // Outputs the sorted array
        for (int i = 0; i < len; i++) {
            sorted_num[i].write(num[i].read());
        }
    }

    // Swaps two elements in the array
    void swap_num(int idx1, int idx2) {
        int temp = num[idx1];
        num[idx1] = num[idx2];
        num[idx2] = temp;
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
