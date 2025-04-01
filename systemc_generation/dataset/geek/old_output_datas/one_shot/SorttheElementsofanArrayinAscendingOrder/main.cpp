
#include <systemc.h>

// Module to sort an array using bubble sort
SC_MODULE(SortArray) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> rst;  // Reset signal
    sc_in<int> nums[5];  // Input array
    sc_out<int> sorted_nums[5];  // Output sorted array
    sc_out<bool> done;  // Signal indicating sorting is complete

    int arr[5];  // Internal array to hold the input and sorted data
    bool is_sorted;  // Flag to indicate if the array is sorted

    // Constructor
    SC_CTOR(SortArray) {
        SC_METHOD(sort_process);
        sensitive << clk.posedge_event() << rst.posedge_event();
    }

    // Process to sort the array
    void sort_process() {
        if (rst.read()) {
            // Initialize array and flags on reset
            for (int i = 0; i < 5; ++i) {
                arr[i] = nums[i].read();
            }
            is_sorted = false;
            done.write(false);
        } else if (!is_sorted) {
            // Bubble sort logic
            bool swapped = false;
            for (int i = 0; i < 5; ++i) {
                for (int j = 1; j < 5 - i; ++j) {
                    if (arr[j] < arr[j - 1]) {
                        // Swap elements
                        int temp = arr[j];
                        arr[j] = arr[j - 1];
                        arr[j - 1] = temp;
                        swapped = true;
                    }
                }
                if (!swapped) {
                    is_sorted = true;
                    break;
                }
            }
            if (is_sorted) {
                // Write sorted array to output ports
                for (int i = 0; i < 5; ++i) {
                    sorted_nums[i].write(arr[i]);
                }
                done.write(true);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk;  // Clock signal
    sc_signal<bool> rst;  // Reset signal
    sc_signal<int> nums[5];  // Input array
    sc_signal<int> sorted_nums[5];  // Output sorted array
    sc_signal<bool> done;  // Signal indicating sorting is complete

    SortArray sort_inst;  // Instance of the SortArray module

    // Constructor
    SC_CTOR(Testbench) : sort_inst("sort_inst") {
        // Connect signals to ports
        sort_inst.clk(clk);
        sort_inst.rst(rst);
        for (int i = 0; i < 5; ++i) {
            sort_inst.nums[i](nums[i]);
            sort_inst.sorted_nums[i](sorted_nums[i]);
        }
        sort_inst.done(done);

        // Process to run tests
        SC_THREAD(run_tests);
        SC_THREAD(generate_clock);
    }

    // Thread to run test cases
    void run_tests() {
        // Reset the system
        rst.write(true);
        wait(2, SC_NS);
        rst.write(false);

        // Initialize input array
        nums[0].write(1);
        nums[1].write(12);
        nums[2].write(6);
        nums[3].write(8);
        nums[4].write(10);

        // Wait for sorting to complete
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Print sorted array
        cout << "Sorted array: ";
        for (int i = 0; i < 5; ++i) {
            cout << sorted_nums[i].read() << " ";
        }
        cout << endl;

        // Stop the simulation
        sc_stop();
    }

    // Thread to generate clock signal
    void generate_clock() {
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

    // Start the simulation
    sc_start();

    return 0;
}
