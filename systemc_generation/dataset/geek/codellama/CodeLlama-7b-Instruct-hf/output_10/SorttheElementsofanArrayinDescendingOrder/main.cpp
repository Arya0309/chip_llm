scss
#include <systemc.h>

// Module that sorts an input array of integers in ascending order

SC_MODULE(BubbleSort) {
    sc_in<int> array_in[MAX_ARRAY_SIZE]; // Input port for the array
    sc_in<int> array_len; // Input port for the length of the array
    sc_buffer<int> sorted_array; // Output port for the sorted array

    // Process to perform sorting
    SC_PROCESS(sort);

    // Constructor
    SC_CTOR(BubbleSort) {
        // Connect input and output ports
        sorted_array.bind(array_out);
    }

    // Sorting process
    void sort() {
        // Initialize a flag to keep track if any swaps were made in the last iteration
        bool is_swapped = false;

        // Outer loop to iterate over the array
        while (true) {
            // Inner loop to iterate over the array from the current index to the end
            for (int i = 0; i < array_len; i++) {
                // Check if the current element is smaller than its predecessor
                if (array_in[i] < array_in[i - 1]) {
                    // Swap the current element and its predecessor if the former is smaller
                    swap(array_in[i], array_in[i - 1]);
                    // Set the flag to signal that a swap was made
                    is_swapped = true;
                }
            }

            // If no swaps were made in the last iteration, exit the outer loop
            if (!is_swapped) {
                break;
            }
        }
    }

    // Swap two elements in the array
    void swap(int& a, int& b) {
        int temp = a;
        a = b;
        b = temp;
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
