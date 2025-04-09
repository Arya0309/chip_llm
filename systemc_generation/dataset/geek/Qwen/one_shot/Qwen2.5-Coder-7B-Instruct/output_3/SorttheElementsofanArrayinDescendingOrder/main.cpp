
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;
    sc_out<int> sorted_array[5]; // Assuming the array size is known

    int nums[5] = {1, 12, 6, 8, 10};
    bool isSwapped;

    SC_CTOR(BubbleSort) {
        SC_METHOD(sort);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(check_done);
        sensitive << sorted_array[0] << sorted_array[1] << sorted_array[2] << sorted_array[3] << sorted_array[4];
        dont_initialize();
    }

    void sort() {
        while (!start.read()) wait(); // Wait for start signal

        for (int i = 0; i < 5; i++) {
            isSwapped = false;
            for (int j = 1; j < 5 - i; j++) {
                if (nums[j] < nums[j - 1]) {
                    swapNums(j, j - 1);
                    isSwapped = true;
                }
            }
            if (!isSwapped) break;
        }

        // Copy sorted array to output ports
        for (int i = 0; i < 5; i++) {
            sorted_array[i].write(nums[i]);
        }

        done.write(true); // Signal sorting completion
    }

    void swapNums(int first, int second) {
        int curr = nums[first];
        nums[first] = nums[second];
        nums[second] = curr;
    }

    void check_done() {
        static bool first_time = true;
        if (first_time) {
            first_time = false;
            return;
        }

        done.write(false); // Reset done signal
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
