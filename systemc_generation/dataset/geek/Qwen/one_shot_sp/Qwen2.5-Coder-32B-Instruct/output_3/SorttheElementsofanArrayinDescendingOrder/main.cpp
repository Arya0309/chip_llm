
#include <systemc.h>

// BubbleSort Module: Implements the bubble sort algorithm

SC_MODULE(BubbleSort) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Signal to start sorting
    sc_out<bool> done;    // Signal to indicate sorting is complete

    // Internal storage for the array (fixed size of 5 elements)
    int arr[5];

    // Constructor: register the process
    SC_CTOR(BubbleSort) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
    }

    // Process that waits for the start signal and performs sorting
    void sort_process() {
        done.write(false); // Initialize done signal to false
        while (true) {
            wait(); // Wait for a clock cycle
            if (start.read() == true) {
                bubbleSort(); // Perform sorting
                done.write(true);
                wait(); // Wait for one cycle to signal completion
                done.write(false);
            }
        }
    }

    // Bubble sort algorithm implementation (sorts the internal array)
    void bubbleSort() {
        // Number of elements is fixed at 5
        const int n = 5;
        for (int i = 0; i < n - 1; i++) {
            bool isSwapped = false;
            for (int j = 1; j < n - i; j++) {
                if (arr[j] < arr[j - 1]) {
                    // Swap arr[j] and arr[j-1]
                    int temp = arr[j];
                    arr[j] = arr[j - 1];
                    arr[j - 1] = temp;
                    isSwapped = true;
                }
            }
            if (!isSwapped) {
                break;
            }
        }
    }

    // Helper method to load an input array into the module
    void load_array(const int input[5]) {
        for (int i = 0; i < 5; i++) {
            arr[i] = input[i];
        }
    }

    // Helper method to read the sorted array from the module
    void read_array(int output[5]) {
        for (int i = 0; i < 5; i++) {
            output[i] = arr[i];
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
