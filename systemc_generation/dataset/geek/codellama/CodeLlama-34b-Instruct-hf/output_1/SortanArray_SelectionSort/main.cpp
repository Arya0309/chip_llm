
#include <systemc.h>

// Define the input and output signals
sc_in<int> arr[];
sc_in<int> n;
sc_out<int> done;

// Define the sorted array
int sorted_arr[];
int sorted_n;

// Define the swap function
void swap(int *xp, int *yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Define the selectionSort function
void selectionSort(int arr[], int n) {
    int i, j, min_idx;
    for (i = 0; i < n-1; i++) {
        min_idx = i;
        for (j = i+1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        swap(&arr[min_idx], &arr[i]);
    }
    for (i = 0; i < n; i++) {
        sorted_arr[i] = arr[i];
    }
    sorted_n = n;
    done.write(true);
}

// Define the printArray function
void printArray(int arr[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

// Define the top-level module

SC_MODULE(SelectionSort) {
    // Define the ports
    sc_in<int> clk;
    sc_in<int> start;
    sc_out<int> done;

    // Define the local variables
    int i, j, min_idx;

    // Define the algorithms
    void selectionSort() {
        for (i = 0; i < sorted_n-1; i++) {
            min_idx = i;
            for (j = i+1; j < sorted_n; j++) {
                if (sorted_arr[j] < sorted_arr[min_idx]) {
                    min_idx = j;
                }
            }
            swap(&sorted_arr[min_idx], &sorted_arr[i]);
        }
        done.write(true);
    }

    // Define the reset process
    void reset() {
        sorted_n = 0;
        for (i = 0; i < n; i++) {
            sorted_arr[i] = arr[i];
        }
        done.write(false);
    }

    // Define the sensitivity list
    sc_sensitive << clk.pos() << start;

    // Define the method
    void method() {
        if (start.read()) {
            selectionSort();
        } else if (reset_event) {
            reset();
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
