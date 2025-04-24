scss
#include <systemc.h>

// Module to sort an array of integers using the Selection Sort algorithm

SC_MODULE(SelectionSort) {
    // Inputs
    sc_in<bool> clk; // Clock signal
    sc_in<bool> start; // Start signal
    sc_out<bool> done; // Done signal

    // Wires to store the number of elements in the array
    sc_in<int> num_elements;

    // Wires to store the input and output arrays
    sc_in<int> in_array[32];
    sc_out<int> out_array[32];

    void sort() {
        // Run the selection sort algorithm on the input array
        int i, j, min_idx;
        for (i = 0; i < num_elements; i++) {
            min_idx = i;
            for (j = i+1; j < num_elements; j++) {
                if (in_array[j] < in_array[min_idx]) {
                    min_idx = j;
                }
            }
            swap(in_array[min_idx], in_array[i]);
        }

        // Output the sorted array
        for (i = 0; i < num_elements; i++) {
            out_array[i] = in_array[i];
        }
    }

    SC_METHOD(sort)
    sensitive << clk.pos() << start.pos();

    // Acknowledge start signal and set done signal
    SC_CTOR(SelectionSort) {
        SC_CTHREAD(sort, clk.pos());
        reset_signal_is(done, 0);
    }

    void swap(int *xp, int *yp) {
        int temp = *xp;
        *xp = *yp;
        *yp = temp;
    }


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
