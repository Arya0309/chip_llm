
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> start;  // Start signal
    sc_out<bool> done;  // Done signal
    sc_out<int> sorted_array[5];  // Output port for the sorted array

    int nums[5] = {1, 12, 6, 8, 10};  // Array to be sorted
    bool isSwapped;
    int temp;

    // Constructor
    SC_CTOR(BubbleSort) {
        // Process to handle the sorting
        SC_METHOD(sort);
        sensitive << clk.pos();
        dont_initialize();

        // Process to handle the start and done signals
        SC_METHOD(handle_signals);
        sensitive << start;
    }

    // Method to sort the array
    void sort() {
        while (true) {
            wait();  // Wait for the clock edge
            if (start.read()) {
                for (int i = 0; i < 5; i++) {
                    isSwapped = false;
                    for (int j = 1; j < 5 - i; j++) {
                        if (nums[j] < nums[j - 1]) {
                            temp = nums[j];
                            nums[j] = nums[j - 1];
                            nums[j - 1] = temp;
                            isSwapped = true;
                        }
                    }
                    if (!isSwapped) {
                        break;
                    }
                }
                done.write(true);  // Signal that sorting is done
            }
        }
    }

    // Method to handle start and done signals
    void handle_signals() {
        while (true) {
            wait();  // Wait for the start signal
            if (start.read()) {
                for (int i = 0; i < 5; i++) {
                    sorted_array[i].write(nums[i]);  // Write sorted array to output
                }
            }
        }
    }
};


SC_MODULE(Testbench) {
    sc_clock clk;          // Clock signal for synchronization
    sc_signal<bool> start; // Signal to trigger the sort
    sc_signal<bool> done;  // Signal indicating sort completion

    // Instance of the BubbleSort module
    BubbleSort* bubble_sort_inst;

    SC_CTOR(Testbench) : clk("clk", 1, SC_NS) {
        bubble_sort_inst = new BubbleSort("bubble_sort_inst");
        bubble_sort_inst->clk(clk);
        bubble_sort_inst->start(start);
        bubble_sort_inst->done(done);

        SC_THREAD(run_tests);
    }

    // Process to run the test cases
    void run_tests() {
        // Initialize the array with unsorted values {1, 12, 6, 8, 10}
        int arr_in[5] = {1, 12, 6, 8, 10};
        bubble_sort_inst->load_array(arr_in);

        // Print the array before sorting
        cout << "Before sorting: ";
        for (int i = 0; i < 5; i++) {
            cout << arr_in[i] << " ";
        }
        cout << endl;

        // Start the sorting process
        start.write(true);
        wait(1, SC_NS); // wait for one clock cycle
        start.write(false);

        // Wait until the BubbleSort module signals that sorting is complete
        while (done.read() != true) {
            wait(1, SC_NS);
        }
        
        // Retrieve the sorted array from the BubbleSort module
        int arr_out[5];
        bubble_sort_inst->read_array(arr_out);

        // Print the sorted array
        cout << "After sorting: ";
        for (int i = 0; i < 5; i++) {
            cout << arr_out[i] << " ";
        }
        cout << endl;

        // Expected sorted result is {1, 6, 8, 10, 12}
        int expected[5] = {1, 6, 8, 10, 12};
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
