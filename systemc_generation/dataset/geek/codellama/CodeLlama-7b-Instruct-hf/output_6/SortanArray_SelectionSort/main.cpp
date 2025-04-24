
#include <systemc.h>


SC_MODULE(SelectionSort) {

    // Input ports
    sc_in<int> arr[MAX_SIZE];  // Input array

    // Output port
    sc_out<int> sorted[MAX_SIZE];  // Sorted array

    // Local variables
    int n;  // Array size
    int i;  // Loop variable
    int j;  // Loop variable
    int min_idx;  // Index of smallest element
    int temp;  // Temp variable for swap

    // Function to find the smallest element in the unsorted array
    int findMin(int arr[MAX_SIZE]) {
        int min_idx = 0;
        for (int j=1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;
        return min_idx;
    }

    // Function to sort the array in ascending order
    void sort(int arr[MAX_SIZE]) {
        for (int i=0; i < n-1; i++) {
            min_idx = findMin(arr);
            temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }

    // Method to print the sorted array
    void printSorted(int arr[MAX_SIZE]) {
        for (int i=0; i < n; i++)
            sorted[i] = arr[i];
    }

    // Constructor
    SC_CTOR(SelectionSort()) {
        // Process to sort the array
        SC_METHOD(sort);
        sensitive << arr;

        // Process to print the sorted array
        SC_METHOD(printSorted);
        sensitive << sorted;
    }
};

int main() {

    // Declare an instance of the SelectionSort module
    SelectionSort selectionSort;

    // Declare a signal for the input array
    sc_signal<int> input[MAX_SIZE];

    // Declare a signal for the sorted array
    sc_signal<int> sorted[MAX_SIZE];

    // Connect the input signal to the input port of the SelectionSort module
    selectionSort.arr(input);

    // Call the SelectionSort function to sort the array
    selectionSort.sort(input);

    // Get the sorted array from the output port of the SelectionSort module
    sorted = selectionSort.sorted();

    // Print the sorted array
    cout << "Sorted array: ";
    for (int i=0; i < n; i++)
        cout << sorted[i] << " ";
    cout << endl;

    // Return 0 to indicate successful completion
    return 0;
}


SC_MODULE(Testbench) {
    sc_clock clk;              // Clock signal for synchronization
    sc_signal<bool> start;     // Signal to trigger the sort
    sc_signal<bool> done;      // Signal indicating sort completion

    // Instance of the SelectionSort module
    SelectionSort* selection_sort_inst;

    SC_CTOR(Testbench) : clk("clk", 1, SC_NS) {
        selection_sort_inst = new SelectionSort("selection_sort_inst");
        selection_sort_inst->clk(clk);
        selection_sort_inst->start(start);
        selection_sort_inst->done(done);

        SC_THREAD(run_tests);
    }

    // Process to run the test cases
    void run_tests() {
        // Initialize the array with unsorted values {64, 25, 12, 22, 11}
        int arr_in[5] = {64, 25, 12, 22, 11};
        selection_sort_inst->load_array(arr_in);

        // Start the sorting process
        start.write(true);
        wait(1, SC_NS); // Wait for one clock cycle
        start.write(false);

        // Wait until the SelectionSort module signals that sorting is done
        while (done.read() != true) {
            wait(1, SC_NS);
        }
        
        // Retrieve the sorted array from the SelectionSort module
        int arr_out[5];
        selection_sort_inst->read_array(arr_out);

        // Expected sorted result is {11, 12, 22, 25, 64}
        int expected[5] = {11, 12, 22, 25, 64};

        // Verify that each element is sorted as expected
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
