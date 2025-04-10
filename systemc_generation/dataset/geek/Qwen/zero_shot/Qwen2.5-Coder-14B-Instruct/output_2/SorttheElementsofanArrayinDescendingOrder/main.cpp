
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;
    sc_vector<sc_signal<int>> nums;
    sc_signal<int> size_nums;
    sc_signal<bool> isSwapped;
    sc_signal<int> i, j;

    SC_CTOR(BubbleSort) : nums("nums", 5) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        SC_THREAD(print_process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void sort_process() {
        while (true) {
            wait(); // Wait for the rising edge of the clock
            if (start.read()) {
                size_nums.write(5); // Assuming fixed size for simplicity
                for (i.write(0); i.read() < size_nums.read(); i.write(i.read() + 1)) {
                    isSwapped.write(false);
                    for (j.write(1); j.read() < size_nums.read() - i.read(); j.write(j.read() + 1)) {
                        if (nums[j.read()].read() < nums[j.read() - 1].read()) {
                            swapNums(j.read(), j.read() - 1);
                            isSwapped.write(true);
                        }
                    }
                    if (!isSwapped.read()) {
                        break;
                    }
                }
                done.write(true);
            } else {
                done.write(false);
            }
        }
    }

    void print_process() {
        while (true) {
            wait(); // Wait for the rising edge of the clock
            if (start.read()) {
                cout << "Before sorting the array is: \n";
                for (i.write(0); i.read() < size_nums.read(); i.write(i.read() + 1)) {
                    cout << nums[i.read()].read() << " ";
                }
                cout << "\n\n";

                wait(); // Wait for the sorting process to complete
                cout << "After sorting the array is: \n";
                for (i.write(0); i.read() < size_nums.read(); i.write(i.read() + 1)) {
                    cout << nums[i.read()].read() << " ";
                }
                cout << "\n";
            }
        }
    }

    void swapNums(int first, int second) {
        int curr = nums[first].read();
        nums[first].write(nums[second].read());
        nums[second].write(curr);
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
