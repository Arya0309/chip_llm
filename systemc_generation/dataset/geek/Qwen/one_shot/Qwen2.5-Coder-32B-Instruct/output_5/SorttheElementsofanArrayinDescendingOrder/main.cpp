
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;       // Clock signal
    sc_in<bool> start;     // Start signal to initiate sorting
    sc_out<bool> done;      // Done signal to indicate sorting completion

    // Internal array and its size
    static const int SIZE = 5;
    int nums[SIZE];

    // Constructor
    SC_CTOR(BubbleSort) {
        // Process to perform sorting
        SC_METHOD(sort_process);
        sensitive << clk.pos();  // Sensitive to positive edge of clock
        dont_initialize();       // Do not initialize on construction
    }

    void sort_process() {
        static bool isSwapping = false;
        static int outerLoop = 0;
        static int innerLoop = 1;
        static bool isSwapped = false;

        if (start.read()) {
            // Initialize array
            nums[0] = 1;
            nums[1] = 12;
            nums[2] = 6;
            nums[3] = 8;
            nums[4] = 10;

            isSwapping = true;
            outerLoop = 0;
            innerLoop = 1;
            isSwapped = false;
        }

        if (isSwapping) {
            if (innerLoop < SIZE - outerLoop) {
                if (nums[innerLoop] < nums[innerLoop - 1]) {
                    // Swap elements
                    int temp = nums[innerLoop];
                    nums[innerLoop] = nums[innerLoop - 1];
                    nums[innerLoop - 1] = temp;
                    isSwapped = true;
                }
                innerLoop++;
            } else {
                if (!isSwapped) {
                    isSwapping = false;
                    done.write(true);  // Sorting completed
                } else {
                    outerLoop++;
                    innerLoop = 1;
                    isSwapped = false;
                }
            }
        } else {
            done.write(false);  // Not sorting
        }
    }
};

// Example Testbench


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
