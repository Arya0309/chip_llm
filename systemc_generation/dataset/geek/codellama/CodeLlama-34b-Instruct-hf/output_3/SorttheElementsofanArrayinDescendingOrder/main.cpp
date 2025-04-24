scss
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk; // Input clock
    sc_in<bool> start; // Input trigger
    sc_in<bool> done; // Output trigger
    sc_in<int> num_in[10]; // Input array
    sc_out<int> num_out[10]; // Output array

    // Process to perform sorting
    SC_METHOD(sort) {
        sensitive << clk.posedge(); // Triggered on clock edge

        // Initialize variables
        int i = 0;
        int j = 1;
        int k = 0;
        int temp;

        // BubbleSort the array
        while (i < 10) {
            if (num_in[j] < num_in[i]) {
                // Swap elements
                temp = num_in[i];
                num_in[i] = num_in[j];
                num_in[j] = temp;
            }
            i++;
            j++;
            if (j == 10) {
                j = 0;
            }
        }

        // Update output array
        for (k = 0; k < 10; k++) {
            num_out[k] = num_in[k];
        }

        // Set done signal
        done.write(true);
    }

    // Constructor
    SC_CTOR(BubbleSort) {
        clk(clk);
        start(start);
        done(done);
        num_in(num_in);
        num_out(num_out);
    }

    // Method to reset the system
    void reset() {
        num_in.write(0);
        num_out.write(0);
        done.write(false);
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
