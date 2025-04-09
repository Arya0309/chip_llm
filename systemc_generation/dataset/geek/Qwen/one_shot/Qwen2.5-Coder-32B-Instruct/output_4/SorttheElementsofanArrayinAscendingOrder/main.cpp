
#include <systemc.h>

// Module that sorts an array using bubble sort

SC_MODULE(BubbleSort) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal
    sc_out<bool> done;     // Done signal

    int nums[5];          // Array to be sorted
    int size_nums;        // Size of the array
    bool isSwapped;        // Flag to check if any swap was made
    int i, j;              // Loop counters

    // Constructor
    SC_CTOR(BubbleSort) {
        size_nums = 5;    // Initialize size of the array
        nums[0] = 1;      // Initialize array values
        nums[1] = 12;
        nums[2] = 6;
        nums[3] = 8;
        nums[4] = 10;

        // Process to perform sorting
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        // Process to handle start signal
        SC_METHOD(start_process);
        sensitive << start;
    }

    // Method to handle start signal
    void start_process() {
        if (start.read()) {
            i = 0;
            j = 1;
            isSwapped = false;
            done.write(false);
        }
    }

    // Method to perform sorting
    void sort_process() {
        if (start.read()) {
            if (i < size_nums) {
                if (j < size_nums - i) {
                    if (nums[j] < nums[j - 1]) {
                        int temp = nums[j];
                        nums[j] = nums[j - 1];
                        nums[j - 1] = temp;
                        isSwapped = true;
                    }
                    j++;
                } else {
                    if (!isSwapped) {
                        done.write(true);
                    } else {
                        isSwapped = false;
                        i++;
                        j = 1;
                    }
                }
            } else {
                done.write(true);
            }
        }
    }
};

// Testbench module


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
        // Initialize the array with unsorted values {1, 12, 6, 8, 10}
        int arr_in[5] = {1, 12, 6, 8, 10};
        bubble_sort_inst->load_array(arr_in);

        // Print the array before sorting
        cout << "Before sorting: ";
        for (int i = 0; i < 5; i++) {
            cout << arr_in[i] << " ";
        }
        cout << endl;

        // Start the sorting process
        start.write(true);
        wait(1, SC_NS); // wait for one clock cycle
        start.write(false);

        // Wait until the BubbleSort module signals that sorting is complete
        while (done.read() != true) {
            wait(1, SC_NS);
        }
        
        // Retrieve the sorted array from the BubbleSort module
        int arr_out[5];
        bubble_sort_inst->read_array(arr_out);

        // Print the sorted array
        cout << "After sorting: ";
        for (int i = 0; i < 5; i++) {
            cout << arr_out[i] << " ";
        }
        cout << endl;

        // Expected sorted result is {1, 6, 8, 10, 12}
        int expected[5] = {1, 6, 8, 10, 12};
        for (int i = 0; i < 5; i++) {
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
