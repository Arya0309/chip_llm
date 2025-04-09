
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;     // Clock signal
    sc_in<bool> start;   // Start signal to initiate sorting
    sc_out<bool> done;    // Done signal indicating sorting completion
    sc_vector<sc_signal<int>> nums; // Input/output vector for array elements
    sc_signal<int> size_nums; // Signal for the size of the array

    // Internal variables
    sc_signal<int> index_i;
    sc_signal<int> index_j;
    sc_signal<bool> isSwapped;
    sc_signal<bool> sorting_done;

    // Constructor
    SC_CTOR(BubbleSort) : nums("nums", 5) {
        // Initialize signals
        index_i.initialize(0);
        index_j.initialize(1);
        isSwapped.initialize(false);
        sorting_done.initialize(false);

        // Process to handle sorting
        SC_THREAD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        // Process to set done signal
        SC_THREAD(done_process);
        sensitive << clk.pos();
        dont_initialize();
    }

    // Sorting process
    void sort_process() {
        while (true) {
            wait(); // Wait for clock edge
            if (start.read()) {
                // Reset internal signals
                index_i.write(0);
                index_j.write(1);
                isSwapped.write(false);
                sorting_done.write(false);

                while (!sorting_done.read()) {
                    wait(); // Wait for clock edge
                    if (index_j.read() < size_nums.read() - index_i.read()) {
                        if (nums[index_j.read()].read() < nums[index_j.read() - 1].read()) {
                            // Swap elements
                            int temp = nums[index_j.read()].read();
                            nums[index_j.read()].write(nums[index_j.read() - 1].read());
                            nums[index_j.read() - 1].write(temp);
                            isSwapped.write(true);
                        }
                        index_j.write(index_j.read() + 1);
                    } else {
                        if (!isSwapped.read()) {
                            sorting_done.write(true);
                        } else {
                            index_i.write(index_i.read() + 1);
                            index_j.write(1);
                            isSwapped.write(false);
                        }
                    }
                }
            }
        }
    }

    // Process to set done signal
    void done_process() {
        while (true) {
            wait(); // Wait for clock edge
            if (sorting_done.read()) {
                done.write(true);
            } else {
                done.write(false);
            }
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
