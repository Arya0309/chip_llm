
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal
    sc_out<bool> done;   // Done signal
    sc_in<int> nums[5];   // Input array
    sc_out<int> sorted_nums[5];  // Output sorted array

    // Internal signals
    bool isSwapped;
    int temp;

    // Constructor
    SC_CTOR(BubbleSort) {
        // Process to handle sorting
        SC_METHOD(sort);
        sensitive << clk.pos();
        dont_initialize();
    }

    // Method to sort the array
    void sort() {
        if (start.read()) {
            for (int i = 0; i < 5; i++) {
                isSwapped = false;
                for (int j = 1; j < 5 - i; j++) {
                    if (nums[j].read() < nums[j - 1].read()) {
                        temp = nums[j].read();
                        sorted_nums[j].write(temp);
                        sorted_nums[j - 1].write(nums[j - 1].read());
                        isSwapped = true;
                    }
                }
                if (!isSwapped) {
                    break;
                }
            }
            done.write(true);
        } else {
            done.write(false);
        }
    }

    // Method to read the sorted array
    void read_array(int arr[]) {
        for (int i = 0; i < 5; i++) {
            arr[i] = sorted_nums[i].read();
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
