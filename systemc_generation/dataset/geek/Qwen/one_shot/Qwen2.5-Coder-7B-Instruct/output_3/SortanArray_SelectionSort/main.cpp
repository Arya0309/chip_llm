
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;     // Clock signal
    sc_in<bool> start;   // Start signal
    sc_out<bool> done;   // Done signal
    sc_out<int> sorted_array[5];  // Output array

    int arr[5] = {64, 25, 12, 22, 11};  // Predefined array
    int n = 5;                            // Size of the array
    bool running = false;                 // Flag to indicate if sorting is running
    int i = 0, j = 0, min_idx = 0;         // Indices for the selection sort algorithm

    // Constructor
    SC_CTOR(SelectionSort) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();
    }

    // Method to perform the selection sort algorithm
    void sort_process() {
        if (start.read()) {
            running = true;
        }

        if (running) {
            if (i < n-1) {
                if (j == i+1 && j < n) {
                    if (arr[j] < arr[min_idx]) {
                        min_idx = j;
                    }
                    j++;
                } else if (j == n) {
                    swap(&arr[min_idx], &arr[i]);
                    i++;
                    j = i+1;
                    min_idx = i;
                } else {
                    j = i+1;
                    min_idx = i;
                }
            } else {
                running = false;
                done.write(true);
            }
        }

        // Copy the sorted array to the output ports
        for (int k = 0; k < n; k++) {
            sorted_array[k].write(arr[k]);
        }
    }

    // Helper function to swap elements
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
