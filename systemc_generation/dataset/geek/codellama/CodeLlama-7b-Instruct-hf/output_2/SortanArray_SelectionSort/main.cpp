
#include <systemc.h>

// Module that sorts an array of integers using the selection sort algorithm

SC_MODULE(SelectionSort) {
    // Input ports
    sc_in<int> arr;
    sc_in<int> n;

    // Output port
    sc_out<int> sorted_arr;

    // Constructor
    SC_CTOR(SelectionSort) {
        // Process to perform selection sort
        SC_METHOD(selection_sort);
        sensitive << arr << n;
    }

    // Selection sort algorithm
    void selection_sort() {
        int i, j, min_idx;

        // Iterate through the array from 0 to n-1
        for (i = 0; i < n.read() - 1; i++) {

            // Find the minimum element in the unsorted part of the array
            min_idx = i;
            for (j = i + 1; j < n.read(); j++) {
                if (arr.read()[j] < arr.read()[min_idx]) {
                    min_idx = j;
                }
            }

            // Swap the minimum element with the first element of the unsorted part
            swap(arr.read()[min_idx], arr.read()[i]);
        }
    }

    // Swap function
    void swap(int x, int y) {
        int temp;
        temp = x;
        x = y;
        y = temp;
    }

    // Function to print the sorted array
    void print_array() {
        for (int i = 0; i < n.read(); i++) {
            cout << arr.read()[i] << " ";
        }
        cout << endl;
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
