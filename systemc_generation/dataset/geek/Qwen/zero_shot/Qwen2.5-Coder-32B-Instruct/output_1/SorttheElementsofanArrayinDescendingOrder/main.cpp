
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    SC_CTOR(BubbleSort) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(done_process);
        sensitive << clk.pos();
        dont_initialize();
    }

private:
    enum State {IDLE, SORTING, DONE};
    State state;
    int nums[5]; // Fixed size array for simplicity
    int len;
    bool isSwapped;
    int i, j;

    void sort_process() {
        if (start.read()) {
            switch (state) {
                case IDLE:
                    // Initialize array and variables
                    nums[0] = 1; nums[1] = 12; nums[2] = 6; nums[3] = 8; nums[4] = 10;
                    len = 5;
                    i = 0;
                    state = SORTING;
                    break;

                case SORTING:
                    if (i < len) {
                        if (j < len - i) {
                            if (nums[j] < nums[j - 1]) {
                                swapNums(j, j - 1);
                                isSwapped = true;
                            }
                            j++;
                        } else {
                            if (!isSwapped) {
                                state = DONE;
                            } else {
                                i++;
                                j = 1;
                                isSwapped = false;
                            }
                        }
                    }
                    break;

                case DONE:
                    break;
            }
        }
    }

    void done_process() {
        if (start.read() && state == DONE) {
            done.write(true);
        } else {
            done.write(false);
        }
    }

    void swapNums(int first, int second) {
        int curr = nums[first];
        nums[first] = nums[second];
        nums[second] = curr;
    }
};



SC_MODULE(Testbench) {
    sc_clock clk;          // Clock signal for synchronization
    sc_signal<bool> start; // Signal to trigger sorting
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

    // Process to run the test cases.
    void run_tests() {
        // Initialize the array with unsorted values {1, 12, 6, 8, 10}.
        int arr_in[5] = {1, 12, 6, 8, 10};
        bubble_sort_inst->load_array(arr_in);

        // Optional: Print array before sorting.
        std::cout << "Before sorting: ";
        for (int i = 0; i < 5; i++) {
            std::cout << arr_in[i] << " ";
        }
        std::cout << std::endl;

        // Start the sorting process.
        start.write(true);
        wait(1, SC_NS); // Wait one clock cycle.
        start.write(false);

        // Wait until the bubble sort module signals that sorting is done.
        while (done.read() != true) {
            wait(1, SC_NS);
        }
        
        // Retrieve the sorted array from the BubbleSort module.
        int arr_out[5];
        bubble_sort_inst->read_array(arr_out);

        // Expected sorted result is {1, 6, 8, 10, 12}.
        int expected[5] = {1, 6, 8, 10, 12};

        // Optional: Print sorted array.
        std::cout << "After sorting: ";
        for (int i = 0; i < 5; i++) {
            std::cout << arr_out[i] << " ";
        }
        std::cout << std::endl;

        // Verify that each element is sorted as expected.
        for (int i = 0; i < 5; i++) {
            assert(arr_out[i] == expected[i]);
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // End simulation.
    }
};

// sc_main: Entry point for the SystemC simulation.
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create the testbench instance.
    sc_start();         // Start the simulation.
    return 0;
}
