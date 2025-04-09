
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;       // Clock signal
    sc_in<bool> start;     // Start signal
    sc_out<bool> done;     // Done signal
    sc_inout<int*> data;   // Pointer to the array data

    int size_nums;         // Size of the array

    // Constructor
    SC_CTOR(BubbleSort) : size_nums(5), data(nullptr) {
        // Initialize the array
        data = new int[size_nums];
        data[0] = 1;
        data[1] = 12;
        data[2] = 6;
        data[3] = 8;
        data[4] = 10;

        // Process to handle sorting
        SC_THREAD(sort_array);
        sensitive << clk.pos();
    }

    ~BubbleSort() {
        delete[] data;
    }

    // Thread to perform sorting
    void sort_array() {
        while (true) {
            wait(clk.pos()); // Wait for clock edge

            if (start.read()) {
                bool isSwapped;
                for (int i = 0; i < size_nums; i++) {
                    isSwapped = false;
                    for (int j = 1; j < size_nums - i; j++) {
                        if (data[j] < data[j - 1]) {
                            int temp = data[j];
                            data[j] = data[j - 1];
                            data[j - 1] = temp;
                            isSwapped = true;
                        }
                    }
                    if (!isSwapped) {
                        break;
                    }
                }
                done.write(true); // Signal sorting is done
            } else {
                done.write(false); // Reset done signal
            }
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
