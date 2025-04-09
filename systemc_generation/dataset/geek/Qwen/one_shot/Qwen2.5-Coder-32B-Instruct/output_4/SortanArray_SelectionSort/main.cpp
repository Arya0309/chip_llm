
#include <systemc.h>

// Module that performs selection sort

SC_MODULE(SelectionSort) {
    sc_in<bool> clk;         // Clock signal
    sc_in<bool> start;       // Start signal to begin sorting
    sc_out<bool> done;       // Signal indicating completion of sorting

    sc_in<int> arr_size;     // Size of the array
    sc_in<int> arr_in[10];   // Input array (fixed size for simplicity)
    sc_out<int> arr_out[10]; // Output sorted array

    int local_arr[10];        // Local copy of the input array
    int n;                    // Size of the array
    bool sorting;             // Flag to indicate sorting is in progress
    int i, j, min_idx;        // Loop indices and minimum index

    // Constructor
    SC_CTOR(SelectionSort) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();

        SC_METHOD(copy_array);
        sensitive << start.pos();
    }

    // Method to copy input array to local array
    void copy_array() {
        if (start.read()) {
            sorting = true;
            done.write(false);
            n = arr_size.read();
            for (int k = 0; k < n; k++) {
                local_arr[k] = arr_in[k].read();
            }
            i = 0;
        }
    }

    // Method to perform sorting
    void sort_process() {
        if (sorting) {
            if (i < n-1) {
                min_idx = i;
                for (j = i+1; j < n; j++) {
                    if (local_arr[j] < local_arr[min_idx]) {
                        min_idx = j;
                    }
                }
                // Swap the found minimum element with the first element
                int temp = local_arr[min_idx];
                local_arr[min_idx] = local_arr[i];
                local_arr[i] = temp;
                i++;
            } else {
                sorting = false;
                done.write(true);
                // Write sorted array to output ports
                for (int k = 0; k < n; k++) {
                    arr_out[k].write(local_arr[k]);
                }
            }
        }
    }
};

// Example usage


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
