
#include <systemc.h>


SC_MODULE(BubbleSort) {

// Input ports for the array and its size
sc_in<int> num[5];
sc_in<int> size_nums;

// Output port to indicate when the sort is done
sc_out<bool> done;

// Process to sort the array
SC_METHOD(sort);
sensitive << num << size_nums;

void sort() {

// Initializing an array with some integers
int nums[] = {1, 12, 6, 8, 10};
int size_nums = (sizeof(nums)/sizeof(nums[0]));

cout << "Before sorting the array is: \n";
for (int i = 0; i < size_nums; i++) {
    cout << num[i] << " ";
}
cout << "\n\n";

/*
Function to sort the array
for each step, Max item will come at the
last index and swap element if the 
element is smaller than the previous one.
*/
for (int i = 0; i < size_nums; i++) {
    bool isSwapped = false;
    isSwapped = false;
    for (int j = 1; j < size_nums - i; j++) {
        if (num[j] < num[j - 1]) {
            swapNums(num[j], num[j - 1]);
            isSwapped = true;
        }
    }
    if (!isSwapped) {
        break;
    }
}
done.write(true);

cout << "After sorting the array is: \n";

for (int i = 0; i < size_nums; i++) {
    cout << num[i] << " ";
}
cout << "\n";
}

void swapNums() {

// Swap two number
num[j] = num[j - 1];
num[j - 1] = num[j];

done.write(false);
}
}



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
