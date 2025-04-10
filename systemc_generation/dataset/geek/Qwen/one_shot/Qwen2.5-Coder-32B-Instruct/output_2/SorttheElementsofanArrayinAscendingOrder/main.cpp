
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal to begin sorting
    sc_out<bool> done;    // Done signal to indicate completion

    sc_signal<sc_uint<8>> nums[5];  // Array to hold the numbers
    sc_uint<8> sorted_nums[5];     // Array to store the sorted numbers

    int size_nums = 5;             // Size of the array
    int i, j;                      // Loop counters
    bool isSwapped;                // Flag to check if any swapping occurred

    SC_CTOR(BubbleSort) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();     // Sensitivity to positive edge of clock
    }

    void sort_process() {
        while (true) {
            wait();                 // Wait for the next clock cycle
            if (start.read()) {
                // Initialize the array with some values
                nums[0].write(1);
                nums[1].write(12);
                nums[2].write(6);
                nums[3].write(8);
                nums[4].write(10);

                i = 0;
                isSwapped = false;

                while (i < size_nums) {
                    isSwapped = false;
                    for (j = 1; j < size_nums - i; j++) {
                        if (nums[j].read() < nums[j - 1].read()) {
                            swapNums(j, j - 1);
                            isSwapped = true;
                        }
                    }
                    if (!isSwapped) {
                        break;
                    }
                    i++;
                }

                // Copy sorted numbers to sorted_nums
                for (int k = 0; k < size_nums; k++) {
                    sorted_nums[k] = nums[k].read();
                }

                // Indicate sorting is done
                done.write(true);
            } else {
                done.write(false);
            }
        }
    }

    void swapNums(int first, int second) {
        sc_uint<8> curr = nums[first].read();
        nums[first].write(nums[second].read());
        nums[second].write(curr);
    }
};

// Example Testbench


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
