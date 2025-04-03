
#include <systemc.h>

// Module to perform selection sort
SC_MODULE(SelectionSortModule) {
    sc_in<int> clk;          // Clock signal
    sc_in<bool> rst_n;        // Active-low reset signal
    sc_in<bool> start;        // Start signal to begin sorting
    sc_inout<int> arr[5];     // Array of integers (fixed size for simplicity)
    sc_out<bool> done;        // Done signal indicating sorting completion

    int n = 5;                // Size of the array
    int i, j, min_idx;        // Loop indices and minimum index
    bool sorting;             // Flag to indicate if sorting is ongoing

    // Constructor
    SC_CTOR(SelectionSortModule) {
        // Process to handle sorting
        SC_METHOD(sort_process);
        sensitive << clk.pos() << rst_n.neg();
    }

    // Sorting process
    void sort_process() {
        if (!rst_n.read()) {
            // Reset state
            i = 0;
            sorting = false;
            done.write(false);
        } else if (start.read() && !sorting) {
            // Start sorting
            i = 0;
            sorting = true;
            done.write(false);
        } else if (sorting) {
            // Perform selection sort
            if (i < n - 1) {
                min_idx = i;
                for (j = i + 1; j < n; j++) {
                    if (arr[j].read() < arr[min_idx].read())
                        min_idx = j;
                }
                // Swap elements
                int temp = arr[i].read();
                arr[i].write(arr[min_idx].read());
                arr[min_idx].write(temp);
                i++;
            } else {
                // Sorting completed
                sorting = false;
                done.write(true);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> clk;          // Clock signal
    sc_signal<bool> rst_n;        // Active-low reset signal
    sc_signal<bool> start;        // Start signal to begin sorting
    sc_signal<int> arr[5];        // Array of integers (fixed size for simplicity)
    sc_signal<bool> done;         // Done signal indicating sorting completion

    SelectionSortModule sorter;  // Instance of the SelectionSortModule

    // Constructor
    SC_CTOR(Testbench) : sorter("sorter") {
        // Connect signals to ports
        sorter.clk(clk);
        sorter.rst_n(rst_n);
        sorter.start(start);
        sorter.done(done);
        for (int i = 0; i < 5; ++i) {
            sorter.arr[i](arr[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
        sensitive << clk.pos();
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize array
        arr[0].write(64);
        arr[1].write(25);
        arr[2].write(12);
        arr[3].write(22);
        arr[4].write(11);

        // Reset system
        rst_n.write(false);
        wait(2, SC_NS);
        rst_n.write(true);
        wait(2, SC_NS);

        // Start sorting
        start.write(true);
        wait(1, SC_NS);
        start.write(false);

        // Wait until sorting is done
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Print sorted array
        cout << "Sorted array: ";
        for (int i = 0; i < 5; ++i) {
            cout << arr[i].read() << " ";
        }
        cout << endl;

        // Stop the simulation
        sc_stop();
    }

    // Clock generation
    void clock_gen() {
        while (true) {
            clk.write(false);
            wait(1, SC_NS);
            clk.write(true);
            wait(1, SC_NS);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Generate clock
    sc_thread(clock_gen);
    tb.clock_gen();

    // Start the simulation
    sc_start();

    return 0;
}
