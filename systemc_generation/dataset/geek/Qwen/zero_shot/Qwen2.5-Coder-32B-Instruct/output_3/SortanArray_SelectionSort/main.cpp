
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    SC_CTOR(SelectionSort) : state(IDLE), index_i(0), index_j(0), min_idx(0), array_size(5) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(done_process);
        sensitive << state;
    }

private:
    enum StateType { IDLE, FIND_MIN, SWAP, DONE };
    StateType state;

    int array[5];
    int index_i, index_j, min_idx, array_size;

    void sort_process() {
        if (start.read()) {
            switch (state) {
                case IDLE:
                    // Initialize array and indices
                    array[0] = 64; array[1] = 25; array[2] = 12; array[3] = 22; array[4] = 11;
                    index_i = 0;
                    state = FIND_MIN;
                    break;
                case FIND_MIN:
                    if (index_j < array_size) {
                        if (array[index_j] < array[min_idx]) {
                            min_idx = index_j;
                        }
                        index_j++;
                    } else {
                        state = SWAP;
                    }
                    break;
                case SWAP:
                    if (min_idx != index_i) {
                        int temp = array[min_idx];
                        array[min_idx] = array[index_i];
                        array[index_i] = temp;
                    }
                    index_i++;
                    if (index_i < array_size - 1) {
                        index_j = index_i + 1;
                        min_idx = index_i;
                        state = FIND_MIN;
                    } else {
                        state = DONE;
                    }
                    break;
                case DONE:
                    // Do nothing, wait for next start signal
                    break;
            }
        }
    }

    void done_process() {
        if (state == DONE) {
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
