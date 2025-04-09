
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal
    sc_out<bool> done;     // Done signal

    sc_in<int> nums_in[5]; // Input array ports (fixed size for simplicity)
    sc_out<int> nums_out[5];// Output array ports (fixed size for simplicity)

    int nums[5];          // Internal storage for the array
    int size_nums = 5;    // Size of the array

    bool isSwapped;       // Flag to check if any swap was made

    // Constructor
    SC_CTOR(BubbleSort) {
        SC_METHOD(sort_process);
        sensitive << clk.pos(); // Sensitivity to positive edge of clock
    }

    // Sorting process method
    void sort_process() {
        static int i = 0, j = 1; // Static variables to keep track of loop indices
        static int stage = 0;    // Stage 0: Initialization, Stage 1: Sorting, Stage 2: Completion

        switch (stage) {
            case 0: // Initialization
                if (start.read()) {
                    for (int k = 0; k < size_nums; ++k) {
                        nums[k] = nums_in[k].read(); // Read input array into internal storage
                    }
                    i = 0;
                    j = 1;
                    isSwapped = false;
                    stage = 1; // Move to sorting stage
                }
                break;

            case 1: // Sorting
                if (i < size_nums) {
                    if (j < size_nums - i) {
                        if (nums[j] < nums[j - 1]) {
                            swapNums(j, j - 1); // Swap elements if needed
                            isSwapped = true;
                        }
                        j++;
                    } else {
                        if (!isSwapped) {
                            stage = 2; // No swaps, sorting complete
                        } else {
                            i++;
                            j = 1;
                            isSwapped = false;
                        }
                    }
                }
                break;

            case 2: // Completion
                for (int k = 0; k < size_nums; ++k) {
                    nums_out[k].write(nums[k]); // Write sorted array to output ports
                }
                done.write(true); // Signal completion
                stage = 0; // Reset for next start signal
                break;
        }
    }

    // Swap function
    void swapNums(int first, int second) {
        int curr = nums[first];
        nums[first] = nums[second];
        nums[second] = curr;
    }
};

// Example usage in a testbench
#include <iostream>
using namespace std;



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
