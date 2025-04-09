
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal to initiate sorting
    sc_out<bool> done;     // Done signal indicating sorting completion

    // Internal variables
    sc_vector<sc_signal<int>> nums; // Vector to hold the array elements
    sc_signal<int> size_nums;       // Size of the array
    sc_signal<int> index_i;         // Outer loop index
    sc_signal<int> index_j;         // Inner loop index
    sc_signal<bool> isSwapped;     // Flag to check if any swap happened
    sc_signal<bool> sorting_done;  // Flag to indicate sorting completion

    // Constructor
    SC_CTOR(BubbleSort) : nums("nums", 5) { // Initialize nums vector with size 5
        // Reset signals
        SC_METHOD(reset);
        sensitive << clk.pos();

        // Start sorting process
        SC_METHOD(sort_process);
        sensitive << clk.pos() << start;

        // Swap operation
        SC_METHOD(swap_process);
        sensitive << clk.pos();
    }

    // Method to reset internal signals
    void reset() {
        if (clk.posedge()) {
            index_i = 0;
            index_j = 1;
            isSwapped = false;
            sorting_done = false;
        }
    }

    // Method to handle the sorting process
    void sort_process() {
        if (clk.posedge()) {
            if (start) {
                if (index_i < size_nums) {
                    if (index_j < size_nums - index_i) {
                        if (nums[index_j].read() < nums[index_j - 1].read()) {
                            isSwapped = true;
                        } else {
                            isSwapped = false;
                        }
                    } else {
                        if (!isSwapped) {
                            sorting_done = true;
                        }
                        index_i++;
                        index_j = 1;
                    }
                } else {
                    sorting_done = true;
                }
            }
        }
    }

    // Method to perform the swap operation
    void swap_process() {
        if (clk.posedge()) {
            if (isSwapped) {
                int temp = nums[index_j].read();
                nums[index_j].write(nums[index_j - 1].read());
                nums[index_j - 1].write(temp);
                isSwapped = false;
            }
        }
    }

    // Method to set the initial array and size
    void set_array(const int arr[], int len) {
        size_nums = len;
        for (int i = 0; i < len; i++) {
            nums[i].write(arr[i]);
        }
    }

    // Method to get the sorted array
    void get_sorted_array(int arr[]) {
        for (int i = 0; i < size_nums.read(); i++) {
            arr[i] = nums[i].read();
        }
    }
};

// Example usage in a testbench


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
