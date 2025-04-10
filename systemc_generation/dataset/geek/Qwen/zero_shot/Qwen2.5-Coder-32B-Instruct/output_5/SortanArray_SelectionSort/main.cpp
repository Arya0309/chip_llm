
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Internal signals and variables
    sc_signal<int> arr[5]; // Assuming a fixed size of 5 for simplicity
    int n = 5;
    int i, j, min_idx;
    bool sorting_done;

    SC_CTOR(SelectionSort) {
        sorting_done = false;
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(done_process);
        sensitive << sorting_done;
        dont_initialize();
    }

    void sort_process() {
        if (start.read()) {
            if (!sorting_done) {
                if (i < n - 1) {
                    min_idx = i;
                    for (j = i + 1; j < n; j++) {
                        if (arr[j].read() < arr[min_idx].read())
                            min_idx = j;
                    }
                    // Swap the found minimum element with the first element
                    int temp = arr[min_idx].read();
                    arr[min_idx].write(arr[i].read());
                    arr[i].write(temp);

                    i++;
                } else {
                    sorting_done = true;
                }
            }
        }
    }

    void done_process() {
        if (sorting_done) {
            done.write(true);
        } else {
            done.write(false);
        }
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
