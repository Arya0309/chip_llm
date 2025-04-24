scss
#include <systemc.h>

/**
 * @brief Sorts an array of integers in ascending order
 * using a simple bubble sort algorithm.
 */

SC_MODULE(BubbleSort) {
    // Input ports
    sc_in<int> num[ARRAY_LENGTH];
    sc_in<int> len;

    // Output port
    sc_out<int> sorted_num[ARRAY_LENGTH];

    SC_CTOR(BubbleSort) {
        SC_METHOD(sort);
        sensitive << num << len;
    }

    /**
     * @brief Sorts the input array in ascending order.
     */
    void sort() {
        // Initialize the sorted array
        for (int i = 0; i < ARRAY_LENGTH; i++) {
            sorted_num[i] = num[i];
        }

        // Sort the array using bubble sort
        bool is_swapped = true;
        while (is_swapped) {
            is_swapped = false;
            for (int i = 0; i < ARRAY_LENGTH-1; i++) {
                if (sorted_num[i] > sorted_num[i+1]) {
                    swapNums(sorted_num[i], sorted_num[i+1]);
                    is_swapped = true;
                }
            }
        }
    }

    /**
     * @brief Swap two numbers in the sorted array.
     *
     * @param first The first number to swap.
     * @param second The second number to swap.
     */
    void swapNums(int first, int second) {
        int temp = first;
        first = second;
        second = temp;
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
