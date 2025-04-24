scss
#include <systemc.h>


SC_MODULE(BubbleSort)
{
    sc_in<int> nums[];  // Input port for the array
    sc_out<int> sorted_nums[];  // Output port for the sorted array
    sc_in<int> len;  // Input port for the length of the array
    sc_out<bool> done;  // Output port for the sorted array
    sc_in<bool> clk;  // Input port for the clock signal
    sc_in<bool> start;  // Input port for the start signal

    // Constructor
    SC_CTOR(BubbleSort)
    {
        // Process to perform sorting
        SC_METHOD(sort);
        sensitive << nums << len << clk << start;
    }

    // Method to sort the array
    void sort()
    {
        int i = 0;
        int j = 1;
        bool isSwapped = false;
        int temp;
        int size = len;

        // Loop until the length of the array is 1
        while (size > 1)
        {
            // Loop through the array and swap elements
            for (i = 0; i < size - 1; i++, j++)
            {
                if (nums[j] < nums[j - 1])
                {
                    temp = nums[j];
                    nums[j] = nums[j - 1];
                    nums[j - 1] = temp;
                    isSwapped = true;
                }
            }

            // If no swap occurred, the array is sorted
            if (!isSwapped)
            {
                done.write(true);
                break;
            }

            size = size - 1;
        }

        sorted_nums.write(nums);
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
