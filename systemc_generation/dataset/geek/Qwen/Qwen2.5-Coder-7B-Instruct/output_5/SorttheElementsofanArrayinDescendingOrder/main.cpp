
#include <systemc.h>


SC_MODULE(BubbleSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    sc_in<int> arr_in[5];  // Assuming fixed size for simplicity
    sc_out<int> arr_out[5];

    SC_CTOR(BubbleSort) {
        SC_THREAD(sort_array);
        sensitive << clk.pos();
    }

    void sort_array() {
        while (true) {
            wait(start.pos());
            int nums[5];
            for (int i = 0; i < 5; i++) {
                nums[i] = arr_in[i].read();
            }

            cout << "Before sorting the array is: \n";
            for (int i = 0; i < 5; i++)
                cout << nums[i] << " ";
            cout << "\n\n";

            sort(nums, 5);

            cout << "After sorting the array is: \n";
            for (int i = 0; i < 5; i++)
                cout << nums[i] << " ";
            cout << "\n";

            for (int i = 0; i < 5; i++) {
                arr_out[i].write(nums[i]);
            }

            done.write(true);
            wait(clk.pos());
            done.write(false);
        }
    }

    void sort(int num[], int len) {
        bool isSwapped;

        for (int i = 0; i < len; i++) {
            isSwapped = false;
            for (int j = 1; j < len - i; j++) {
                if (num[j] < num[j - 1]) {
                    swapNums(num, j, (j - 1));
                    isSwapped = true;
                }
            }
            if (!isSwapped) {
                break;
            }
        }
    }

    void swapNums(int nums[], int first, int second) {
        int curr = nums[first];
        nums[first] = nums[second];
        nums[second] = curr;
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
