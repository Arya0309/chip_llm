
#include <systemc.h>

// Define the maximum size of the array
static const int MAX_SIZE = 5;

// Module that performs bubble sort

SC_MODULE(BubbleSort) {
    // Input and output ports
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;
    
    // Input and output array
    sc_in<int> nums_in[MAX_SIZE];
    sc_out<int> nums_out[MAX_SIZE];
    
    // Internal variables
    int nums[MAX_SIZE];
    bool isSwapped;
    int i, j;
    bool sorting_done;
    
    SC_CTOR(BubbleSort) {
        // Initialize the internal array from input ports
        SC_METHOD(init_array)
            .sensitive << start.pos();
        
        // Process for sorting the array
        SC_METHOD(sort_process)
            .sensitive << clk.pos();
        
        // Process to signal completion
        SC_METHOD(done_process)
            .sensitive << clk.pos();
    }
    
    // Method to initialize the array from input ports
    void init_array() {
        for (int k = 0; k < MAX_SIZE; k++) {
            nums[k] = nums_in[k].read();
        }
        i = 0;
        j = 1;
        isSwapped = false;
        sorting_done = false;
        done.write(false);
    }
    
    // Method to perform the sorting process
    void sort_process() {
        if (!sorting_done && start.read()) {
            if (i < MAX_SIZE) {
                if (j < MAX_SIZE - i) {
                    if (nums[j] < nums[j - 1]) {
                        int temp = nums[j];
                        nums[j] = nums[j - 1];
                        nums[j - 1] = temp;
                        isSwapped = true;
                    }
                    j++;
                } else {
                    if (!isSwapped) {
                        sorting_done = true;
                    } else {
                        i++;
                        j = 1;
                        isSwapped = false;
                    }
                }
            }
        }
    }
    
    // Method to write the sorted array to output ports and set done signal
    void done_process() {
        if (sorting_done) {
            for (int k = 0; k < MAX_SIZE; k++) {
                nums_out[k].write(nums[k]);
            }
            done.write(true);
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
