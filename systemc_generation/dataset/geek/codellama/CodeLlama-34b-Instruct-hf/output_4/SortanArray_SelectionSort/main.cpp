
#include <systemc.h>

// Module that represents the selection sort algorithm

SC_MODULE(SelectionSort) {
    sc_in<bool> clk;  // clock signal
    sc_in<bool> start;  // start signal
    sc_out<bool> done;  // done signal

    // Array of integers to sort
    sc_in<int> arr[10];

    // Number of elements in the array
    sc_in<int> n;

    // Constructor
    SC_CTOR(SelectionSort) {
        // Process to perform selection sort
        SC_METHOD(sort);
        sensitive << clk.pos() << start.pos();
    }

    // Selection sort algorithm
    void sort() {
        int i, j, min_idx;

        // Initialize the first element of the array as the minimum element
        min_idx = 0;

        // Iterate through the array and find the minimum element in the unsorted part of the array
        for (i = 1; i < n; i++) {
            if (arr[i] < arr[min_idx]) {
                min_idx = i;
            }
        }

        // Swap the minimum element with the first element of the array
        swap(arr, min_idx, 0);

        // Move the boundary of the unsorted subarray by one position
        for (j = 1; j < n; j++) {
            swap(arr, j, j-1);
        }

        // Repeat steps 2 to 4 until the entire array is sorted
        for (i = 2; i < n; i++) {
            for (j = i; j < n; j++) {
                if (arr[j] < arr[min_idx]) {
                    min_idx = j;
                }
            }
            for (j = i; j < n; j++) {
                swap(arr, j, j-1);
            }
        }

        // Signal completion
        done.write(true);
    }
};


SC_MODULE(Testbench) {
    sc_clock clk;              // Clock signal for synchronization
    sc_signal<bool> start;     // Signal to trigger the sort
    sc_signal<bool> done;      // Signal indicating sort completion

    // Instance of the SelectionSort module
    SelectionSort* selection_sort_inst;

    SC_CTOR(Testbench) : clk("clk", 1, SC_NS) {
        selection_sort_inst = new SelectionSort("selection_sort_inst");
        selection_sort_inst->clk(clk);
        selection_sort_inst->start(start);
        selection_sort_inst->done(done);

        SC_THREAD(run_tests);
    }

    // Process to run the test cases
    void run_tests() {
        // Initialize the array with unsorted values {64, 25, 12, 22, 11}
        int arr_in[5] = {64, 25, 12, 22, 11};
        selection_sort_inst->load_array(arr_in);

        // Start the sorting process
        start.write(true);
        wait(1, SC_NS); // Wait for one clock cycle
        start.write(false);

        // Wait until the SelectionSort module signals that sorting is done
        while (done.read() != true) {
            wait(1, SC_NS);
        }
        
        // Retrieve the sorted array from the SelectionSort module
        int arr_out[5];
        selection_sort_inst->read_array(arr_out);

        // Expected sorted result is {11, 12, 22, 25, 64}
        int expected[5] = {11, 12, 22, 25, 64};

        // Verify that each element is sorted as expected
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
