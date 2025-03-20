
#include <systemc.h>
#include <algorithm> // For std::sort

// Module to merge two sorted arrays
SC_MODULE(MergeSortedArrays) {
    // Input ports for the sizes of the arrays
    sc_in<int> n1, n2;

    // Input ports for the arrays
    sc_in<int> arr1[4], arr2[4];

    // Output port for the merged array
    sc_out<int> arr3[8];

    // Process to merge and sort the arrays
    void mergeAndSort() {
        int local_arr1[4], local_arr2[4], local_arr3[8];
        
        // Copy input arrays to local variables
        for (int i = 0; i < n1.read(); ++i)
            local_arr1[i] = arr1[i].read();
        for (int i = 0; i < n2.read(); ++i)
            local_arr2[i] = arr2[i].read();

        // Merge arrays
        int i = 0, j = 0, k = 0;
        while (i < n1.read()) {
            local_arr3[k++] = local_arr1[i++];
        }
        while (j < n2.read()) {
            local_arr3[k++] = local_arr2[j++];
        }

        // Sort the merged array
        std::sort(local_arr3, local_arr3 + n1.read() + n2.read());

        // Write the sorted merged array to output
        for (int i = 0; i < n1.read() + n2.read(); ++i)
            arr3[i].write(local_arr3[i]);
    }

    // Constructor to register the process
    SC_CTOR(MergeSortedArrays) {
        SC_METHOD(mergeAndSort);
        sensitive << n1 << n2;
        for (int i = 0; i < 4; ++i) {
            sensitive << arr1[i];
            sensitive << arr2[i];
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MergeSortedArrays
    sc_signal<int> n1, n2;
    sc_signal<int> arr1[4], arr2[4];
    sc_signal<int> arr3[8];

    // Instance of MergeSortedArrays
    MergeSortedArrays merger;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize arrays
        arr1[0] = 1; arr1[1] = 3; arr1[2] = 5; arr1[3] = 7;
        arr2[0] = 2; arr2[1] = 4; arr2[2] = 6; arr2[3] = 8;
        n1 = 4;
        n2 = 4;

        // Wait for one delta cycle to let the merger process
        wait(1, SC_NS);

        // Print the results
        cout << "Array after merging:" << endl;
        for (int i = 0; i < n1.read() + n2.read(); i++)
            cout << arr3[i].read() << " ";
        cout << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : merger("merger") {
        // Connecting signals to the merger ports
        merger.n1(n1); merger.n2(n2);
        for (int i = 0; i < 4; ++i) {
            merger.arr1[i](arr1[i]);
            merger.arr2[i](arr2[i]);
            merger.arr3[i](arr3[i]);
        }
        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
