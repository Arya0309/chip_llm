
#include <systemc.h>

// Module to perform selection sort
SC_MODULE(SelectionSortModule) {
    sc_in<bool> clk;              // Clock signal
    sc_in<bool> reset;            // Reset signal
    sc_vector<sc_in<int>> input;  // Input vector of integers
    sc_vector<sc_out<int>> output;// Output vector of integers
    sc_out<bool> done;            // Done signal indicating sorting completion

    int n;                        // Size of the array
    int arr[10];                  // Internal array to hold the input values
    int i, j, min_idx;            // Indices used in sorting
    bool sorting_done;            // Flag to indicate sorting completion

    // Constructor
    SC_CTOR(SelectionSortModule) : input("input", 10), output("output", 10) {
        // Sensitivity list for the clock and reset
        SC_METHOD(sort_process);
        sensitive << clk.posedge_event() << reset.posedge_event();
    }

    // Sorting process
    void sort_process() {
        if (reset.read()) {
            // Initialize indices and flags on reset
            i = 0;
            j = 0;
            min_idx = 0;
            sorting_done = false;
            done.write(false);
            for (int k = 0; k < n; ++k) {
                arr[k] = input[k].read();
                output[k].write(arr[k]);
            }
        } else if (!sorting_done) {
            // Perform selection sort
            if (i < n - 1) {
                min_idx = i;
                for (j = i + 1; j < n; ++j) {
                    if (arr[j] < arr[min_idx]) {
                        min_idx = j;
                    }
                }
                // Swap the found minimum element with the first element
                int temp = arr[min_idx];
                arr[min_idx] = arr[i];
                arr[i] = temp;
                output[i].write(arr[i]);
                output[min_idx].write(arr[min_idx]);
                i++;
            } else {
                sorting_done = true;
                done.write(true);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk;          // Clock signal
    sc_signal<bool> reset;        // Reset signal
    sc_vector<sc_signal<int>> input;// Input vector of integers
    sc_vector<sc_signal<int>> output;// Output vector of integers
    sc_signal<bool> done;         // Done signal indicating sorting completion

    SelectionSortModule sorter;   // Instance of the SelectionSortModule

    // Constructor
    SC_CTOR(Testbench) : sorter("sorter"), input("input", 10), output("output", 10) {
        // Connect signals to ports
        sorter.clk(clk);
        sorter.reset(reset);
        sorter.done(done);
        for (int i = 0; i < 10; ++i) {
            sorter.input[i](input[i]);
            sorter.output[i](output[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
        SC_METHOD(print_output);
        sensitive << done.posedge_event();
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize the array with some values
        int arr[] = {64, 25, 12, 22, 11};
        n = sizeof(arr) / sizeof(arr[0]);

        // Apply reset
        reset = true;
        wait(1, SC_NS);
        reset = false;
        wait(1, SC_NS);

        // Write input values to the sorter
        for (int i = 0; i < n; ++i) {
            input[i] = arr[i];
        }

        // Wait for sorting to complete
        while (!done.read()) {
            clk = !clk;
            wait(1, SC_NS);
        }

        // Print success message
        cout << "Sorting completed successfully." << endl;

        // Stop the simulation
        sc_stop();
    }

    // Method to print the output array
    void print_output() {
        cout << "Sorted array: ";
        for (int i = 0; i < n; ++i) {
            cout << output[i].read() << " ";
        }
        cout << endl;
    }

    int n; // Size of the array
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
