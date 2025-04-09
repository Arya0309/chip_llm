
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> start;        // Start signal to initiate sorting
    sc_out<bool> done;         // Done signal indicating sorting completion

    sc_in<sc_uint<32>> nums[5];  // Input array of 5 unsigned integers
    sc_out<sc_uint<32>> sorted_nums[5]; // Output sorted array

    sc_uint<32> local_nums[5];  // Local copy of the input array
    bool isSwapped;
    int len;
    int i, j;

    // Constructor
    SC_CTOR(BubbleSort) {
        // Initialize local variables
        len = 5;
        i = 0;
        j = 1;
        isSwapped = false;

        // Registering the sorting process
        SC_METHOD(sort_process);
        sensitive << clk.pos();
    }

    // Sorting process method
    void sort_process() {
        if (start.read()) {
            // Copy input array to local array
            for (int k = 0; k < len; k++) {
                local_nums[k] = nums[k].read();
            }

            // Bubble Sort Logic
            while (i < len) {
                if (j < len - i) {
                    if (local_nums[j] < local_nums[j - 1]) {
                        // Swap elements
                        sc_uint<32> temp = local_nums[j];
                        local_nums[j] = local_nums[j - 1];
                        local_nums[j - 1] = temp;
                        isSwapped = true;
                    }
                    j++;
                } else {
                    if (!isSwapped) {
                        // No swaps occurred, array is sorted
                        done.write(true);
                        break;
                    }
                    // Reset flags and counters for next pass
                    isSwapped = false;
                    i++;
                    j = 1;
                }
            }

            // Write sorted array to output ports
            for (int k = 0; k < len; k++) {
                sorted_nums[k].write(local_nums[k]);
            }
        } else {
            done.write(false);
        }
    }
};

// Example Testbench for BubbleSort Module


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
