
// SystemC equivalent of the provided C++/C code
#include <systemc.h>

// Module that sorts an array of integers

SC_MODULE(BubbleSort) {
    sc_in<int, 16> nums[10]; // Input port for the array to be sorted
    sc_in<int> len; // Input port for the length of the array
    sc_out<int, 16> sorted[10]; // Output port for the sorted array
  
    // Constructor
    SC_CTOR(BubbleSort) {
        // Process to sort the array
        SC_METHOD(sort);
        sensitive << nums << len;
    }
  
    // Method to sort the array
    void sort() {
        // Create a local copy of the input array
        int nums_copy[10] = {0};
        for (int i = 0; i < 10; i++) {
            nums_copy[i] = nums[i].read();
        }
  
        // BubbleSort the array using bubble sort
        bool is_swapped = false;
        for (int i = 0; i < len; i++) {
            for (int j = 1; j < len - i; j++) {
                if (nums_copy[j] < nums_copy[j - 1]) {
                    swapNums(nums_copy, j, (j - 1));
                    is_swapped = true;
                }
            }
            if (!is_swapped) {
                break;
            }
        }
  
        // Write the sorted array to the output port
        for (int i = 0; i < len; i++) {
            sorted[i].write(nums_copy[i]);
        }
    }
  
    // Swaps two elements in the array
    void swapNums(int nums[10], int first, int second) {
        int temp = nums[first];
        nums[first] = nums[second];
        nums[second] = temp;
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
