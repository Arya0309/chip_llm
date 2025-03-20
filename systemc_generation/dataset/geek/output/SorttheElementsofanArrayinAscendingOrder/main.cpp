
#include <systemc.h>

// Module to sort an array using Bubble Sort
SC_MODULE(BubbleSort) {
    // Input port for the array size
    sc_in<int> size_in;
    // Output port for the sorted array
    sc_out<sc_vector<sc_int<32>>> sorted_array_out;
    // Input port for the array to be sorted
    sc_in<sc_vector<sc_int<32>>> array_in;

    // Internal signal to hold the array during sorting
    sc_vector<sc_int<32>> array;
    // Internal signal to indicate completion of sorting
    sc_signal<bool> done;

    // Constructor
    SC_CTOR(BubbleSort) : array("array", 5) {
        // Process to perform sorting
        SC_THREAD(sort_array);
        sensitive << size_in;
    }

    // Method to perform Bubble Sort
    void sort_array() {
        int len = size_in.read();
        bool isSwapped;

        // Copy input array to internal array
        for (int i = 0; i < len; ++i) {
            array[i] = array_in[i].read();
        }

        // Bubble Sort algorithm
        for (int i = 0; i < len; i++) {
            isSwapped = false;
            for (int j = 1; j < len - i; j++) {
                if (array[j] < array[j - 1]) {
                    swap(array[j], array[j - 1]);
                    isSwapped = true;
                }
            }
            if (!isSwapped) {
                break;
            }
        }

        // Write sorted array to output
        for (int i = 0; i < len; ++i) {
            sorted_array_out[i].write(array[i]);
        }

        // Indicate sorting is complete
        done.write(true);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> size_sig; // Signal for the size of the array
    sc_signal<sc_vector<sc_int<32>>> array_sig; // Signal for the array to be sorted
    sc_signal<sc_vector<sc_int<32>>> sorted_array_sig; // Signal for the sorted array

    // Instance of the BubbleSort module
    BubbleSort bubble_sort_inst;

    // Constructor
    SC_CTOR(Testbench) : array_sig("array_sig", 5), sorted_array_sig("sorted_array_sig", 5), bubble_sort_inst("bubble_sort_inst") {
        // Connect signals to module ports
        bubble_sort_inst.size_in(size_sig);
        bubble_sort_inst.array_in(array_sig);
        bubble_sort_inst.sorted_array_out(sorted_array_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize array
        int initial_array[] = {1, 12, 6, 8, 10};
        size_sig.write(5);

        // Write initial array to input signal
        for (int i = 0; i < 5; ++i) {
            array_sig[i].write(initial_array[i]);
        }

        // Wait for sorting to complete
        wait(1, SC_NS);

        // Read sorted array from output signal
        cout << "After sorting the array is: \n";
        for (int i = 0; i < 5; ++i) {
            cout << sorted_array_sig[i].read() << " ";
        }
        cout << "\n";

        // End simulation after successful test
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
