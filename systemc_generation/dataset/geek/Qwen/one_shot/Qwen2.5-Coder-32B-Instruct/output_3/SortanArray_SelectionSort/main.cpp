
#include <systemc.h>

// Module that implements selection sort

SC_MODULE(SelectionSort) {
    sc_in<bool> clk;     // Clock signal
    sc_in<bool> start;   // Start signal
    sc_out<bool> done;   // Done signal

    sc_in<int> arr_size; // Size of the array
    sc_inout<int> arr[10]; // Array of integers (fixed size for simplicity)

    // Internal variables
    int i, j, min_idx;
    bool sorting_done;

    // Constructor
    SC_CTOR(SelectionSort) {
        // Registering the process with sensitivity to the clock's positive edge
        SC_METHOD(sort_process);
        sensitive << clk.pos();

        // Initialization
        i = 0;
        j = 0;
        min_idx = 0;
        sorting_done = false;
    }

    // Sorting process
    void sort_process() {
        if (start.read()) {
            if (!sorting_done) {
                if (i < arr_size.read() - 1) {
                    if (j == i + 1) {
                        min_idx = i;
                    }
                    if (j < arr_size.read()) {
                        if (arr[j].read() < arr[min_idx].read()) {
                            min_idx = j;
                        }
                        j++;
                    } else {
                        // Swap the found minimum element with the first element
                        int temp = arr[min_idx].read();
                        arr[min_idx].write(arr[i].read());
                        arr[i].write(temp);

                        i++;
                        j = i + 1;
                    }
                } else {
                    sorting_done = true;
                }
            }
        } else {
            sorting_done = false;
            i = 0;
            j = 0;
            min_idx = 0;
        }
        done.write(sorting_done);
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
