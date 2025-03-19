
#include <systemc.h>

// Module to perform Selection Sort
SC_MODULE(SelectionSortModule) {
    // Input port for the array size
    sc_in<int> n;
    
    // Input and output ports for the array
    sc_in<int> arr_in[10]; // Assuming maximum array size of 10
    sc_out<int> arr_out[10];

    // Internal array to hold the data during sorting
    int arr[10];
    
    // Process to perform selection sort
    void selectionSortProcess() {
        // Copy input array to internal array
        for (int i = 0; i < n.read(); ++i) {
            arr[i] = arr_in[i].read();
        }

        int i, j, min_idx;

        // Perform selection sort
        for (i = 0; i < n.read() - 1; i++) {
            min_idx = i;
            for (j = i + 1; j < n.read(); j++) {
                if (arr[j] < arr[min_idx]) {
                    min_idx = j;
                }
            }
            // Swap elements
            int temp = arr[min_idx];
            arr[min_idx] = arr[i];
            arr[i] = temp;
        }

        // Write sorted array to output ports
        for (int i = 0; i < n.read(); ++i) {
            arr_out[i].write(arr[i]);
        }
    }

    // Constructor to register the process
    SC_CTOR(SelectionSortModule) {
        SC_METHOD(selectionSortProcess);
        sensitive << n;
        for (int i = 0; i < 10; ++i) {
            sensitive << arr_in[i];
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with SelectionSortModule
    sc_signal<int> n;
    sc_signal<int> arr_in[10];
    sc_signal<int> arr_out[10];

    // Instance of SelectionSortModule
    SelectionSortModule sorter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize array size and array elements
        n = 5;
        arr_in[0] = 64;
        arr_in[1] = 25;
        arr_in[2] = 12;
        arr_in[3] = 22;
        arr_in[4] = 11;

        // Wait for one delta cycle to let the sorter process
        wait(1, SC_NS);

        // Print the sorted array
        cout << "Sorted array: ";
        for (int i = 0; i < n.read(); ++i) {
            cout << arr_out[i].read() << " ";
        }
        cout << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : sorter("sorter") {
        // Connecting signals to the sorter ports
        sorter.n(n);
        for (int i = 0; i < 10; ++i) {
            sorter.arr_in[i](arr_in[i]);
            sorter.arr_out[i](arr_out[i]);
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
