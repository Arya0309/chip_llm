

 #include <systemc.h>

// Module that sorts an array of integers.

SC_MODULE(SelectionSort) {
  sc_in<int> iport;  // Input port for the input array
  sc_out<int> oport; // Output port for the sorted array
  int temp;

  // Constructor
  SC_CTOR(SelectionSort) {
    SC_METHOD(doSort);
    sensitive << iport;
  }

  // Method to perform the actual sorting
  void doSort() {
    int i;

    // Initialize temp with the minimum value
    int temp = 0;
    for (i = 0; i < n; i++) {
      if (temp > arr[i]) {
        temp = arr[i];
      }
  }

  // Swap the minimum value with the ith value
  void swap() {
    int temp2 = arr[i];
    arr[i] = arr[min_idx];
    arr[min_idx] = temp2;
    i++;
  }

  // Sort the array
  void object()
  {
    for (i = 0; i < n; i++) {
      if (temp > arr[i]) {
        temp = arr[i];
      }

      // Swap the ith value with the minimum value
      swap();
    }
    // The final sorted array is written to oport
    oport.write(arr);
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
