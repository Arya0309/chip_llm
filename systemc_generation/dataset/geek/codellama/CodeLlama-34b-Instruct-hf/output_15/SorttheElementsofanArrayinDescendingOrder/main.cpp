scss
#include <systemc.h>

// Define the sort module

SC_MODULE(BubbleSort) {
    // Input ports
    sc_in<sc_int<32>> num[10];        // Integer array to be sorted
    sc_in<sc_int<32>> len;            // Integer array length
    
    // Output port
    sc_out<sc_int<32>> num_sorted[10]; // BubbleSorted integer array
    
    // Constructor
    SC_CTOR(BubbleSort) {
        // BubbleSort the array
        SC_METHOD(sort);
        sensitive << num << len;
    }
    
    // BubbleSort method
    void sort() {
        // Create a temporary array and identical output array
        sc_int<32> tmp_array[10];
        sc_int<32> sorted_array[10];
        for (int i = 0; i < 10; i++) {
            tmp_array[i] = num[i];
        }
        // Use bubble sort to sort the tmp_array
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9 - i; j++) {
                if (tmp_array[j] > tmp_array[j + 1]) {
                    // Swap elements
                    tmp_array[j] = tmp_array[j + 1];
                    tmp_array[j + 1] = tmp_array[j];
                }
            }
        }
        // Copy sorted elements to output array
        for (int i = 0; i < 10; i++) {
            sorted_array[i] = tmp_array[i];
        }
        // Output sorted array
        for (int i = 0; i < 10; i++) {
            num_sorted[i].write(sorted_array[i]);
        }
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
