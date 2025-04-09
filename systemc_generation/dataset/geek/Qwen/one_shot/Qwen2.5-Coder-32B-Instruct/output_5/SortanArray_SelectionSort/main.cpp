
#include <systemc.h>
#include <iostream>

// Module that performs selection sort

SC_MODULE(SelectionSort) {
    sc_in<bool> clk;         // Clock signal
    sc_in<bool> start;       // Start signal to begin sorting
    sc_out<bool> done;       // Done signal indicating sorting completion

    sc_in<sc_uint<8>> arr_in[5];  // Input array elements
    sc_out<sc_uint<8>> arr_out[5]; // Output sorted array elements

    // Internal variables
    sc_uint<8> arr[5];
    int n = 5;
    int i, j, min_idx;
    bool sorting;

    // Constructor
    SC_CTOR(SelectionSort) {
        // Register processes
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(read_input);
        sensitive << start.pos();
        dont_initialize();

        SC_METHOD(write_output);
        sensitive << done.pos();
        dont_initialize();
    }

    // Method to read input array
    void read_input() {
        if (start.read()) {
            for (int k = 0; k < n; k++) {
                arr[k] = arr_in[k].read();
            }
            i = 0;
            j = 0;
            min_idx = 0;
            sorting = true;
            done.write(false);
        }
    }

    // Sorting process
    void sort_process() {
        if (sorting) {
            if (i < n - 1) {
                if (j < n) {
                    if (arr[j] < arr[min_idx]) {
                        min_idx = j;
                    }
                    j++;
                } else {
                    // Swap the found minimum element with the first element
                    sc_uint<8> temp = arr[min_idx];
                    arr[min_idx] = arr[i];
                    arr[i] = temp;

                    i++;
                    j = i + 1;
                    min_idx = i;
                }
            } else {
                sorting = false;
                done.write(true);
            }
        }
    }

    // Method to write output array
    void write_output() {
        if (done.read()) {
            for (int k = 0; k < n; k++) {
                arr_out[k].write(arr[k]);
            }
        }
    }
};

// Testbench for SelectionSort module

SC_MODULE(Top) {
    sc_clock clk;
    sc_signal<bool> start, done;
    sc_signal<sc_uint<8>> arr_in[5], arr_out[5];

    SelectionSort sorter;
    Testbench tb;

    SC_CTOR(Top) : clk("clk", 10, SC_NS), sorter("sorter"), tb("tb") {
        sorter.clk(clk);
        sorter.start(start);
        sorter.done(done);
        tb.clk(clk);
        tb.start(start);
        tb.done(done);

        for (int i = 0; i < 5; i++) {
            sorter.arr_in[i](arr_in[i]);
            sorter.arr_out[i](arr_out[i]);
            tb.arr_in[i](arr_in[i]);
            tb.arr_out[i](arr_out[i]);
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
