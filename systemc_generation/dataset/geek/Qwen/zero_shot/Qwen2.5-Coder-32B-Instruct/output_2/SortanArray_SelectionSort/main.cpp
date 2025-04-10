
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Internal signals and variables
    sc_signal<sc_uint<32>> arr[5]; // Assuming array size is fixed to 5 for simplicity
    sc_signal<int> n;
    sc_signal<int> i, j, min_idx;
    sc_signal<bool> sorting_done;

    SC_CTOR(SelectionSort) {
        SC_METHOD(process);
        sensitive << clk.pos();

        // Initialize the array with some values
        arr[0].write(64);
        arr[1].write(25);
        arr[2].write(12);
        arr[3].write(22);
        arr[4].write(11);
        n.write(5);
        sorting_done.write(false);
    }

    void process() {
        if (start.read()) {
            if (!sorting_done.read()) {
                int local_i = i.read();
                int local_n = n.read();
                if (local_i < local_n - 1) {
                    int local_min_idx = i.read();
                    int local_j = j.read();
                    int local_arr_i = arr[local_i].read();
                    int local_arr_j = arr[local_j].read();

                    if (local_j < local_n) {
                        if (local_arr_j < arr[local_min_idx].read()) {
                            min_idx.write(local_j);
                        }
                        j.write(local_j + 1);
                    } else {
                        // Swap the found minimum element with the first element
                        int local_min_idx_value = arr[min_idx.read()].read();
                        arr[min_idx.read()].write(local_arr_i);
                        arr[local_i].write(local_min_idx_value);

                        i.write(local_i + 1);
                        j.write(local_i + 1);
                    }
                } else {
                    sorting_done.write(true);
                    done.write(true);
                }
            }
        } else {
            done.write(false);
            i.write(0);
            j.write(1);
            sorting_done.write(false);
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
