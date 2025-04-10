
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    sc_vector<sc_signal<int>> arr;
    sc_signal<int> n;
    sc_signal<int> i, j, min_idx;
    sc_signal<bool> running;

    void selection_sort_process() {
        while (true) {
            wait();
            if (start.read()) {
                running.write(true);
                for (i.write(0); i.read() < n.read() - 1; i.write(i.read() + 1)) {
                    min_idx.write(i.read());
                    for (j.write(i.read() + 1); j.read() < n.read(); j.write(j.read() + 1)) {
                        if (arr[j.read()].read() < arr[min_idx.read()].read()) {
                            min_idx.write(j.read());
                        }
                    }
                    // Swap the found minimum element with the first element
                    int temp = arr[min_idx.read()].read();
                    arr[min_idx.read()].write(arr[i.read()].read());
                    arr[i.read()].write(temp);
                }
                running.write(false);
                done.write(true);
            } else if (!running.read()) {
                done.write(false);
            }
        }
    }

    SC_CTOR(SelectionSort) : arr("arr", 5) {
        SC_THREAD(selection_sort_process);
        sensitive << clk.pos();
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
