
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> start;  // Start signal
    sc_out<bool> done; // Done signal
    sc_in<int> input_array[5]; // Input array
    sc_out<int> sorted_array[5]; // Output array

    int arr[5]; // Internal array for sorting
    int n = 5; // Size of the array
    int i, j, min_idx;

    // Constructor
    SC_CTOR(SelectionSort) {
        // Process to handle the sorting logic
        SC_METHOD(sort_process);
        sensitive << clk.pos();
    }

    // Method to handle the sorting logic
    void sort_process() {
        if (start.read()) {
            // Copy input array to internal array
            for (i = 0; i < n; i++) {
                arr[i] = input_array[i].read();
            }

            // One by one move boundary of 
            // unsorted subarray 
            for (i = 0; i < n-1; i++) {
                // Find the minimum element in 
                // unsorted array 
                min_idx = i; 
                for (j = i+1; j < n; j++) 
                    if (arr[j] < arr[min_idx]) 
                        min_idx = j; 

                // Swap the found minimum element 
                // with the first element 
                swap(&arr[min_idx], &arr[i]);
            }

            // Write the sorted array
            for (i = 0; i < n; i++) {
                sorted_array[i].write(arr[i]);
            }

            // Set the done signal
            done.write(true);
        } else {
            done.write(false);
        }
    }

    // Swap function
    void swap(int *xp, int *yp) {
        int temp = *xp; 
        *xp = *yp; 
        *yp = temp; 
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
