
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Signal to start sorting
    sc_out<bool> done;    // Signal to indicate sorting is complete

    // Internal storage for the array (fixed size of 5 elements)
    int arr[5];

    // Constructor: register the process
    SC_CTOR(SelectionSort) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
    }

    // Process that waits for the start signal and performs sorting
    void sort_process() {
        while (true) {
            wait(); // Wait for a clock cycle
            if (start.read() == true) {
                selectionSort(); // Perform sorting
                done.write(true);
                wait(); // Wait for one cycle to signal completion
                done.write(false);
            }
        }
    }

    // Selection sort algorithm implementation (sorts the internal array)
    void selectionSort() {
        int n = sizeof(arr) / sizeof(arr[0]);
        for (int i = 0; i < n - 1; i++) {
            int min_idx = i;
            for (int j = i + 1; j < n; j++) {
                if (arr[j] < arr[min_idx]) {
                    min_idx = j;
                }
            }
            swap(&arr[min_idx], &arr[i]);
        }
    }

    // Helper method to load an input array into the module
    void load_array(const int input[5]) {
        for (int i = 0; i < 5; i++) {
            arr[i] = input[i];
        }
    }

    // Helper method to read the sorted array from the module
    void read_array(int output[5]) {
        for (int i = 0; i < 5; i++) {
            output[i] = arr[i];
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
