#include <systemc.h>

// BubbleSort Module: Implements the bubble sort algorithm
SC_MODULE(BubbleSort) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Signal to start sorting
    sc_out<bool> done;    // Signal to indicate sorting is complete

    // Internal storage for the array (fixed size of 4 elements)
    int arr[4];

    // Constructor: register the process
    SC_CTOR(BubbleSort) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
    }

    // Process that waits for the start signal and performs sorting
    void sort_process() {
        while (true) {
            wait(); // wait for a clock cycle
            if (start.read() == true) {
                bubbleSort(); // perform sorting
                done.write(true);
                wait(); // wait for one cycle to signal completion
                done.write(false);
            }
        }
    }

    // Bubble sort algorithm implementation (sorts the internal array)
    void bubbleSort() {
        // Number of elements is fixed at 4
        const int n = 4;
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    // Swap arr[j] and arr[j+1]
                    int temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }

    // Helper method to load an input array into the module
    void load_array(const int input[4]) {
        for (int i = 0; i < 4; i++) {
            arr[i] = input[i];
        }
    }

    // Helper method to read the sorted array from the module
    void read_array(int output[4]) {
        for (int i = 0; i < 4; i++) {
            output[i] = arr[i];
        }
    }
};

// Testbench Module: Sets up the input array, triggers sorting, and verifies output
SC_MODULE(Testbench) {
    sc_clock clk;          // Clock signal for synchronization
    sc_signal<bool> start; // Signal to trigger the sort
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

    // Process to run the test cases
    void run_tests() {
        // Initialize the array with unsorted values {6, 0, 3, 5}
        int arr_in[4] = {6, 0, 3, 5};
        bubble_sort_inst->load_array(arr_in);

        // Start the sorting process
        start.write(true);
        wait(1, SC_NS); // wait for one clock cycle
        start.write(false);

        // Wait until the bubble sort module signals that sorting is done
        while (done.read() != true) {
            wait(1, SC_NS);
        }
        
        // Retrieve the sorted array from the BubbleSort module
        int arr_out[4];
        bubble_sort_inst->read_array(arr_out);

        // Expected sorted result is {0, 3, 5, 6}
        int expected[4] = {0, 3, 5, 6};

        // Verify that each element is sorted as expected
        for (int i = 0; i < 4; i++) {
            assert(arr_out[i] == expected[i]);
            cout << "Element " << i << " sorted value: " << arr_out[i] << endl;
        }
        cout << "All tests passed successfully." << endl;

        sc_stop(); // End simulation
    }
};

// sc_main: Entry point for the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create the testbench instance
    sc_start();         // Start the simulation
    return 0;
}
