
#include <systemc.h>


SC_MODULE(BubbleSort)
{
    // Inputs
    sc_in<bool> start;
    sc_in<int> nums[10];

    // Outputs
    sc_out<int> BubbleSorted_nums[10];
    sc_out<bool> done;

    // Constants
    const int NUM_ELEMENTS = 10;

    // Methods
    SC_METHOD(BubbleSort_array);
    sensitive << start;

    SC_METHOD(swap_elements);
    sensitive << start;

    // Constructor
    SC_CTOR(BubbleSort)
    {
        // Set initial values
        for (int i = 0; i < NUM_ELEMENTS; i++)
        {
            BubbleSorted_nums[i] <= 0;
            done <= 0;
        }
    }

    // Method to BubbleSort the array
    void BubbleSort_array()
    {
        // Set done to 1 when the BubbleSort operation is complete
        done <= 1;

        // Iterate through each element in the array
        for (int i = 0; i < NUM_ELEMENTS; i++)
        {
            // Iterate through each neighbor of the current element
            for (int j = 1; j < NUM_ELEMENTS - i; j++)
            {
                // If the neighbor is smaller than the current element, swap them
                if (nums[j] < nums[j - 1])
                {
                    swap_elements(j, (j - 1));
                }
            }
        }
    }

    // Method to swap two elements in the array
    void swap_elements(int first, int second)
    {
        // Swap the values of the two elements
        int temp = nums[first];
        nums[first] = nums[second];
        nums[second] = temp;
    }
};


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
