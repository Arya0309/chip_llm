
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<int> data_in;  // Input port for the data to be sorted
    sc_in<int> length;   // Input port for the length of the input array
    sc_out<int> data_out; // Output port for the sorted data

    // Constructor
    SC_CTOR(BubbleSort) {
        // Process to perform bubble sort
        SC_METHOD(sort);
        sensitive << data_in << length;
    }

    // Method to perform bubble sort
    void sort() {
        int data[length]; // Create a new array to hold the data
        int isSwapped;   // Initialize a flag to indicate whether any swaps were made
        int first = 0;   // Initialize the first index to 0
        int second = 1;  // Initialize the second index to 1
        int i;           // Initialize a loop counter

        // Copy the input data to the new array
        for (i = 0; i < length; i++) {
            data[i] = data_in[i];
        }

        // Perform the bubble sort algorithm
        do {
            isSwapped = false;
            for (i = 0; i < length - first; i++) {
                if (data[i] > data[i + 1]) {
                    int temp = data[i];
                    data[i] = data[i + 1];
                    data[i + 1] = temp;
                    isSwapped = true;
                }
            }
            first++;
        } while (isSwapped && (first < length - 1));

        // Copy the sorted data to the output port
        for (i = 0; i < length; i++) {
            data_out[i] = data[i];
        }
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
