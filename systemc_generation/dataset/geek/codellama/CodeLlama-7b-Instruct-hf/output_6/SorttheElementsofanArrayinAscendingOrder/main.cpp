scss
#include <systemc.h>

// Module for the bubble BubbleSort algorithm

SC_MODULE(BubbleSort) {
    sc_in<bool> clk;
    sc_in<sc_uint<32> > nums; // The array to be BubbleSorted
    sc_in<sc_uint<4> > len; // The size of the array
    sc_out<bool> isSwapped; // The flag indicating whether a swap has occurred or not

    SC_METHOD(BubbleSort_array) {
        int i, j, curr;

        for (i = 0; i < len; i++) {
            for (j = 1; j < len - i; j++) {
                if (nums[j] < nums[j - 1]) {
                    curr = nums[j];
                    nums[j] = nums[j - 1];
                    nums[j - 1] = curr;
                    isSwapped.write(true);
                }
            }
        }
        isSwapped.write(false);
    }
};

// Module instantiating the BubbleSort function

SC_MODULE(main) {
    sc_in<bool> clk;
    sc_in<sc_uint<32> > nums; // The array to be BubbleSorted
    sc_in<sc_uint<4> > len; // The size of the array
    sc_out<bool> done;

    BubbleSort s1;

    SC_CTOR(main) {
        s1.clk(clk);
        s1.nums(nums);
        s1.len(len);
        s1.done(done);
        SC_THREAD(BubbleSort_array);
        sensitive << s1.done << s1.isSwapped;
    }

    void BubbleSort_array() {
        bool isSwapped;

        while (isSwapped) {
            isSwapped = s1.isSwapped;
        }
        done.write(true);
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
