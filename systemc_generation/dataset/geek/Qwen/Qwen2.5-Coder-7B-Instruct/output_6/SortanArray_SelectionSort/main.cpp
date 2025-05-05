
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> start;        // Start signal
    sc_out<bool> done;        // Done signal
    sc_inout<int> arr[5];      // Input/output array

    int n;                    // Size of the array
    bool sorting_done;         // Flag to indicate sorting is done

    SC_CTOR(SelectionSort) {
        n = 5;                  // Set the size of the array
        sorting_done = false;

        // Process to handle the sorting logic
        SC_METHOD(sort);
        sensitive << clk.pos();

        // Process to update the output array and set the done signal
        SC_METHOD(update_output);
        sensitive << clk.pos();
    }

    void sort() {
        if (start.read()) {
            for (int i = 0; i < n-1; i++) {
                int min_idx = i;
                for (int j = i+1; j < n; j++) {
                    if (arr[j].read() < arr[min_idx].read()) {
                        min_idx = j;
                    }
                }
                // Swap the values without using pointers
                int temp = arr[min_idx].read();
                arr[min_idx] = arr[i].read();
                arr[i] = temp;
            }
            sorting_done = true;
        }
    }

    void update_output() {
        if (sorting_done) {
            done.write(true);
        } else {
            done.write(false);
        }
    }

    void load_array(int input[]) {
        for (int i = 0; i < n; i++) {
            arr[i] = input[i];
        }
    }

    void read_array(int output[]) {
        for (int i = 0; i < n; i++) {
            output[i] = arr[i].read();
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
