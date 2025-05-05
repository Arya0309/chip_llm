
#include <systemc.h>


SC_MODULE(SelectionSort) {
    sc_in<bool> clk;         // Clock signal
    sc_in<bool> start;       // Start signal
    sc_out<bool> done;      // Done signal
    sc_inout<int> arr[5];     // Input array (assuming array size is 5)

    int n = 5;                // Size of the array

    // Declaration of the sort_method
    void sort_method();

    // Method to perform selection sort
    void sort() {
        int i, j, min_idx;
        for (i = 0; i < n-1; i++) {
            min_idx = i;
            for (j = i+1; j < n; j++)
                if (arr[j].read() < arr[min_idx].read())
                    min_idx = j;
            swap(arr[min_idx], arr[i]);
        }
        done.write(true);
    }

    // Method to swap two array elements
    void swap(sc_inout<int>& xp, sc_inout<int>& yp) {
        int temp = xp.read();
        xp.write(yp.read());
        yp.write(temp);
    }

    // Method to load array elements
    void load_array(const int (&data)[5]) {
        for (int i = 0; i < n; i++) {
            arr[i].write(data[i]);
        }
    }

    // Method to read array elements
    void read_array(int (&data)[5]) {
        for (int i = 0; i < n; i++) {
            data[i] = arr[i].read();
        }
    }

public:
    SC_CTOR(SelectionSort) {
        done.initialize(false);
        SC_METHOD(sort_method);
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
