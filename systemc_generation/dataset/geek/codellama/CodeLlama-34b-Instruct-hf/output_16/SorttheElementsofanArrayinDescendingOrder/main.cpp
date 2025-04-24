
#include <systemc.h>

// Module that sorts an array of integers

SC_MODULE(BubbleSort) {
    sc_in<bool> clk;      // Input clock port
    sc_in<bool> start;    // Input start port
    sc_in<bool> done;     // Output done port
    
    // Input port for the array to be sorted
    sc_in<int> arr_in[];
    
    // Output port for the sorted array
    sc_out<int> arr_out[];
    
    // Flag to indicate whether swapping has been done
    bool isSwapped;
    
    // Method to sort the array
    void sort() {
        // Initialize the flag
        isSwapped = false;
        
        // Iterate through the array
        for (int i = 0; i < arr_in.size(); i++) {
            // Compare the current element with the previous element
            if (arr_in[i] < arr_in[i - 1]) {
                // Swap the elements
                sc_uint<32> temp = arr_in[i];
                arr_in[i] = arr_in[i - 1];
                arr_in[i - 1] = temp;
                
                // Set the flag
                isSwapped = true;
            }
        }
        
        // If swapping has been done, continue sorting
        if (isSwapped) {
            // Iterate through the remaining array elements
            for (int i = 1; i < arr_in.size(); i++) {
                // Compare the current element with the previous element
                if (arr_in[i] < arr_in[i - 1]) {
                    // Swap the elements
                    sc_uint<32> temp = arr_in[i];
                    arr_in[i] = arr_in[i - 1];
                    arr_in[i - 1] = temp;
                    
                    // Set the flag
                    isSwapped = true;
                }
            }
        }
        
        // Print the sorted array
        for (int i = 0; i < arr_in.size(); i++) {
            cout << arr_in[i] << " ";
        }
        cout << "\n";
        
        // Set the done port to true
        done.write(true);
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
