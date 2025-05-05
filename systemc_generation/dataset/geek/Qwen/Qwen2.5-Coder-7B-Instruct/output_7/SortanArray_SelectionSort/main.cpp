
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> start;  // Start signal
    sc_out<bool> done; // Done signal

    int arr[5];  // Array to be sorted
    int n;       // Size of the array
    sc_signal<bool> sort_flag;  // Flag to control sorting
    sc_signal<bool> sort_complete;  // Flag to indicate sorting is complete

    // Constructor
    SC_CTOR(SelectionSort) {
        n = 5;  // Initialize array size
        arr[0] = 64; arr[1] = 25; arr[2] = 12; arr[3] = 22; arr[4] = 11;
        sort_flag.write(false);
        sort_complete.write(false);

        // Process to handle the sorting logic
        SC_METHOD(sort);
        sensitive << clk.pos();
        dont_initialize();

        // Process to handle the start and done signals
        SC_METHOD(start_done);
        sensitive << start;
        dont_initialize();
    }

    // Method to sort the array
    void sort() {
        if (sort_flag.read()) {
            for (int i = 0; i < n-1; i++) {
                int min_idx = i;
                for (int j = i+1; j < n; j++)
                    if (arr[j] < arr[min_idx])
                        min_idx = j;
                swap(&arr[min_idx], &arr[i]);
            }
            sort_complete.write(true);  // Set a flag indicating sorting is complete
        }
    }

    // Method to handle start and done signals
    void start_done() {
        if (start.read() && !sort_flag.read()) {
            sort_flag.write(true);
        } else if (sort_complete.read()) {
            done.write(true);
            sort_flag.write(false);
            sort_complete.write(false);  // Reset the flag after signaling done
        }
    }

    // Method to load input array
    void load_array(const int input[]) {
        for (int i = 0; i < n; i++) {
            arr[i] = input[i];
        }
    }

    // Method to read output array
    void read_array(int output[]) {
        for (int i = 0; i < n; i++) {
            output[i] = arr[i];
        }
    }

    // Helper function to swap elements
    void swap(int *xp, int *yp) {
        int temp = *xp;
        *xp = *yp;
        *yp = temp;
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
