c
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<IntArray> num; // Input port for the array of integers
    sc_in<int> len; // Input port for the number of elements in the array
    sc_out<IntArray> sorted; // Output port for the sorted array

    // Constructor
    BubbleSort(sc_module_name name) : sc_module(name) {
        // Process to sort the array
        SC_METHOD(sort);
        sensitive << num << len;
    }

    // Method to sort the array
    void sort() {
        int size_nums = len.read();
        int curr = num[0];
        IntArray sorted_nums = new IntArray(size_nums);
        for (int i = 0; i < size_nums; i++) {
            sorted_nums[i] = curr;
        }
        for (int i = 0; i < size_nums; i++) {
            for (int j = 1; j < size_nums - i; j++) {
                if (sorted_nums[j] < sorted_nums[j - 1]) {
                    swapNums(sorted_nums, j, (j - 1));
                }
            }
        }
        sorted.write(sorted_nums);
    }

    // Swap two numbers in the array
    void swapNums(IntArray nums, int first, int second) {
        IntArray curr = nums[first];
        nums[first] = nums[second];
        nums[second] = curr;
    }

    // Main function
    void Main() {
        num.write(new IntArray(5));
        len.write(5);
        sorted.write(new IntArray(5));
        sort();
        cout << "BubbleSorted Array: " << sorted.read() << endl;
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
