
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<int> in_array[];
    sc_in<int> len;
    sc_in<bool> clk;
    sc_out<int> out_array[];

    // Clock handling process
    SC_THREAD(sort) {
        // Initialize the minimum index and the current index
        int min_idx = 0;
        int curr_idx = 0;

        // Inner loop to traverse the unsorted part of the array
        SC_LOOP(inner_loop) {
            if (in_array[curr_idx] < in_array[min_idx]) {
                min_idx = curr_idx;
            }
            curr_idx++;
            if (curr_idx == len) {
                break;
            }
        }

        // Wait for the next clock cycle
        wait(clk.posedge_event());

        // Swap the found minimum element with the first element of the unsorted part
        swap(in_array[min_idx], in_array[0]);

        // Increment the starting index of the unsorted part
        start_idx++;

        // Wait for the next clock cycle
        wait(clk.posedge_event());

        // Recursive loop to traverse the sorted part of the array
        SC_LOOP(outer_loop) {
            if (start_idx < len) {
                // Call the sort function on the unsorted part of the array
                sort(in_array, len, clk);
                start_idx++;
            }
            else {
                break;
            }
        }

        // Emit the sorted array
        SC_METHOD(emit_array) {
            SC_CTOR(emit_array) {
                // Iterate over the sorted array and emit each element
                for (int i = 0; i < len; i++) {
                    out_array[i].write(in_array[i].read());
                }
            }
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
