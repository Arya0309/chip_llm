
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal
    sc_out<bool> done;    // Done signal
    sc_out<int> sorted_arr[5]; // Output array

    int arr[5] = {64, 25, 12, 22, 11};
    int n = sizeof(arr)/sizeof(arr[0]);

    void selectionSort();
    void printArray();

    SC_CTOR(SelectionSort) {
        SC_THREAD(selectionSortProcess);
        sensitive << clk.pos();
    }

    void selectionSortProcess() {
        while (true) {
            wait(); // Wait for rising edge of clock
            if (start.read()) {
                selectionSort();
                done.write(true);
                wait(); // Wait for next cycle to indicate completion
                done.write(false);
            }
        }
    }

    void selectionSort() {
        int i, j, min_idx;

        for (i = 0; i < n-1; i++) {
            min_idx = i;
            for (j = i+1; j < n; j++)
                if (arr[j] < arr[min_idx])
                    min_idx = j;

            swap(&arr[min_idx], &arr[i]);
        }

        // Copy sorted array to output ports
        for (i = 0; i < n; i++)
            sorted_arr[i].write(arr[i]);
    }

    void printArray() {
        for (int i = 0; i < n; i++)
            cout << sorted_arr[i].read() << " ";
        cout << endl;
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
