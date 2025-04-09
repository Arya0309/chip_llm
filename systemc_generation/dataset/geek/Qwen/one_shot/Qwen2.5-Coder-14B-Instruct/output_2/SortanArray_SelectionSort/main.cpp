\n#include <systemc.h>\n\n
SC_MODULE(SelectionSort) {\n    sc_in_clk clk;\n    sc_in<bool> start;\n    sc_out<bool> done;\n    sc_vector<sc_in<int>> arr_in;\n    sc_vector<sc_out<int>> arr_out;\n    int n;\n\n    void sort() {\n        int i, j, min_idx;\n        for (i = 0; i < n - 1; i++) {\n            min_idx = i;\n            for (j = i + 1; j < n; j++) {\n                if (arr_in[j].read() < arr_in[min_idx].read()) {\n                    min_idx = j;\n                }\n            }\n            int temp = arr_in[min_idx].read();\n            arr_out[min_idx].write(arr_in[i].read());\n            arr_out[i].write(temp);\n        }\n        done.write(true);\n    }\n\n    SC_CTOR(SelectionSort) : arr_in(\"arr_in\", n), arr_out(\"arr_out\", n) {\n        SC_THREAD(sort);\n        sensitive << start.pos();\n    }\n};\n

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
