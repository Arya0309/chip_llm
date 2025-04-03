
#include <systemc.h>
#include <algorithm> // For std::sort

// Module to merge two sorted arrays
SC_MODULE(MergeArrays) {
    sc_in<int> n1; // Size of first array
    sc_in<int> n2; // Size of second array
    sc_vector<sc_in<int>> arr1; // First sorted array
    sc_vector<sc_in<int>> arr2; // Second sorted array
    sc_vector<sc_out<int>> arr3; // Merged sorted array

    // Constructor
    SC_CTOR(MergeArrays, const sc_module_name& name, int size1, int size2)
        : sc_module(name), arr1("arr1", size1), arr2("arr2", size2), arr3("arr3", size1+size2) {
        // Process to merge arrays
        SC_METHOD(merge);
        sensitive << n1 << n2;
    }

    // Method to merge and sort arrays
    void merge() {
        int i = 0, j = 0, k = 0;
        std::vector<int> temp_arr3(n1.read() + n2.read());

        // Copy elements from arr1
        while(i < n1.read()) {
            temp_arr3[k++] = arr1[i++].read();
        }

        // Copy elements from arr2
        while(j < n2.read()) {
            temp_arr3[k++] = arr2[j++].read();
        }

        // Sort the merged array
        std::sort(temp_arr3.begin(), temp_arr3.end());

        // Write sorted elements to arr3
        for(int l = 0; l < temp_arr3.size(); ++l) {
            arr3[l].write(temp_arr3[l]);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n1; // Size of first array
    sc_signal<int> n2; // Size of second array
    sc_vector<sc_signal<int>> arr1; // First sorted array
    sc_vector<sc_signal<int>> arr2; // Second sorted array
    sc_vector<sc_signal<int>> arr3; // Merged sorted array

    MergeArrays merge_arrays_inst; // Instance of the MergeArrays module

    // Constructor
    SC_CTOR(Testbench) : merge_arrays_inst("merge_arrays", 4, 4), arr1("arr1", 4), arr2("arr2", 4), arr3("arr3", 8) {
        // Connect signals to ports
        merge_arrays_inst.n1(n1);
        merge_arrays_inst.n2(n2);
        for(int i = 0; i < 4; ++i) {
            merge_arrays_inst.arr1[i](arr1[i]);
            merge_arrays_inst.arr2[i](arr2[i]);
            merge_arrays_inst.arr3[i](arr3[i]);
        }
        for(int i = 4; i < 8; ++i) {
            merge_arrays_inst.arr3[i](arr3[i]);
        }

        // Process to initialize and run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize arrays
        n1 = 4;
        n2 = 4;
        arr1[0] = 1; arr1[1] = 3; arr1[2] = 5; arr1[3] = 7;
        arr2[0] = 2; arr2[1] = 4; arr2[2] = 6; arr2[3] = 8;

        // Wait for the merge to complete
        wait(1, SC_NS);

        // Print the merged array
        cout << "Array after merging" << endl;
        for(int i = 0; i < n1.read() + n2.read(); ++i) {
            cout << arr3[i].read() << " ";
        }
        cout << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
