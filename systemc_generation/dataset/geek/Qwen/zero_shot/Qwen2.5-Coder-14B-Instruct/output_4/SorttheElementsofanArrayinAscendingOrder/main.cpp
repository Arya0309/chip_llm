
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Internal variables
    int nums[5];
    int size_nums;
    int index_i, index_j;
    bool isSwapped;
    bool sorting_done;

    SC_CTOR(BubbleSort) : index_i(0), index_j(0), isSwapped(false), sorting_done(false) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void sort_process() {
        while (true) {
            wait(); // Wait for the clock edge
            if (start.read()) {
                // Initialize array and size
                nums[0] = 1; nums[1] = 12; nums[2] = 6; nums[3] = 8; nums[4] = 10;
                size_nums = 5;
                index_i = 0;
                index_j = 0;
                isSwapped = false;
                sorting_done = false;

                // Print initial array
                cout << "Before sorting the array is: ";
                for (int i = 0; i < size_nums; i++) {
                    cout << nums[i] << " ";
                }
                cout << "\n\n";

                // Start sorting process
                while (!sorting_done) {
                    if (index_j < size_nums - index_i - 1) {
                        if (nums[index_j + 1] < nums[index_j]) {
                            // Swap elements
                            int temp = nums[index_j];
                            nums[index_j] = nums[index_j + 1];
                            nums[index_j + 1] = temp;
                            isSwapped = true;
                        }
                        index_j++;
                    } else {
                        if (!isSwapped) {
                            sorting_done = true;
                        } else {
                            isSwapped = false;
                            index_i++;
                            index_j = 0;
                        }
                    }
                }

                // Print sorted array
                cout << "After sorting the array is: ";
                for (int i = 0; i < size_nums; i++) {
                    cout << nums[i] << " ";
                }
                cout << "\n";

                done.write(true); // Indicate sorting is done
            } else {
                done.write(false);
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
