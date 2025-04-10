\n#include <systemc.h>\n\n
SC_MODULE(SelectionSort) {\n\tpublic:\n\t\tsc_in<bool> clk;\n\t\tsc_in<bool> start;\n\t\tsc_out<bool> done;\n\t\tsc_signal<int> arr[5];\n\t\tsc_signal<int> n;\n\t\tsc_signal<int> i, j, min_idx;\n\t\tsc_signal<bool> sorting;\n\t\tsc_signal<bool> swapping;\n\t\tsc_signal<bool> printing;\n\t\tsc_signal<bool> done_flag;\n\n\t\tvoid sort_process() {\n\t\t\twhile (true) {\n\t\t\t\twait();\n\t\t\t\tif (start.read()) {\n\t\t\t\t\tfor (i.write(0); i.read() < n.read() - 1; i.write(i.read() + 1)) {\n\t\t\t\t\t\tmin_idx.write(i.read());\n\t\t\t\t\t\tfor (j.write(i.read() + 1); j.read() < n.read(); j.write(j.read() + 1)) {\n\t\t\t\t\t\t\tif (arr[j.read()].read() < arr[min_idx.read()].read()) {\n\t\t\t\t\t\t\t\tmin_idx.write(j.read());\n\t\t\t\t\t\t\t}\n\t\t\t\t\t\t}\n\t\t\t\t\t\tswapping.write(true);\n\t\t\t\t\t\twait();\n\t\t\t\t\t\tint temp = arr[min_idx.read()].read();\n\t\t\t\t\t\tarr[min_idx.read()].write(arr[i.read()].read());\n\t\t\t\t\t\tarr[i.read()].write(temp);\n\t\t\t\t\t\tswapping.write(false);\n\t\t\t\t\t}\n\t\t\t\t\tdone_flag.write(true);\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\n\t\tvoid print_process() {\n\t\t\twhile (true) {\n\t\t\t\twait();\n\t\t\t\tif (done_flag.read()) {\n\t\t\t\t\tcout << \"Sorted array: \";\n\t\t\t\t\tfor (int k = 0; k < n.read(); k++) {\n\t\t\t\t\t\tcout << arr[k].read() << \" \";\n\t\t\t\t\t}\n\t\t\t\t\tcout << endl;\n\t\t\t\t\tdone_flag.write(false);\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\n\t\tSC_CTOR(SelectionSort) : sorting(false), swapping(false), printing(false), done_flag(false) {\n\t\t\tSC_THREAD(sort_process);\n\t\t\tsensitive << clk.pos();\n\t\t\tSC_THREAD(print_process);\n\t\t\tsensitive << clk.pos();\n\t\t}\n};\n\n

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
