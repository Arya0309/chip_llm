
#include <systemc.h>

// Define constants for array size
static const int ARRAY_SIZE = 5;

// SystemC module for Selection Sort

SC_MODULE(SelectionSort) {
    // Clock and control signals
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Array to be sorted
    sc_signal<int> arr[ARRAY_SIZE];

    // Internal variables
    int i, j, min_idx;
    bool sorting;

    SC_CTOR(SelectionSort) {
        // Initialize internal variables
        i = 0;
        j = 0;
        min_idx = 0;
        sorting = false;
        done.write(false);

        // Process to handle sorting
        SC_METHOD(sort_process);
        sensitive << clk.pos();
    }

    void sort_process() {
        if (start.read()) {
            if (!sorting) {
                // Start sorting process
                i = 0;
                sorting = true;
            }

            if (i < ARRAY_SIZE - 1) {
                if (j == 0) {
                    // Initialize min_idx at the beginning of each outer loop iteration
                    min_idx = i;
                }

                if (j < ARRAY_SIZE) {
                    // Find the minimum element in the unsorted portion
                    if (arr[j].read() < arr[min_idx].read()) {
                        min_idx = j;
                    }
                    j++;
                } else {
                    // Swap the found minimum element with the first element
                    int temp = arr[min_idx].read();
                    arr[min_idx].write(arr[i].read());
                    arr[i].write(temp);

                    // Move to the next element
                    i++;
                    j = i + 1;
                }
            } else {
                // Sorting is complete
                sorting = false;
                done.write(true);
            }
        } else {
            // Reset done signal if not sorting
            done.write(false);
        }
    }
};

// Example testbench to demonstrate the usage of SelectionSort


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
