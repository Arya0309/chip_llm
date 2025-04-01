
#include <systemc.h>
#include <iostream>
#include <vector>

// Module to perform Bubble Sort
SC_MODULE(BubbleSortModule) {
    sc_vector<sc_in<int>> input; // Input ports for the array elements
    sc_vector<sc_out<int>> output; // Output ports for the sorted array elements

    // Constructor
    SC_CTOR(BubbleSortModule, const char* name, int size) : sc_module(name), input("input", size), output("output", size) {
        SC_METHOD(sort);
        sensitive << input;
    }

    // Method to perform Bubble Sort
    void sort() {
        std::vector<int> arr(size());
        for (int i = 0; i < size(); ++i)
            arr[i] = input[i].read();

        for (int i = 0; i < size() - 1; i++) {
            for (int j = 0; j < size() - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    int temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }

        for (int i = 0; i < size(); ++i)
            output[i].write(arr[i]);
    }

    int size() const {
        return input.size();
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_vector<sc_signal<int>> input_signals; // Signals for input array elements
    sc_vector<sc_signal<int>> output_signals; // Signals for output array elements

    BubbleSortModule sorter; // Instance of the BubbleSortModule

    // Constructor
    SC_CTOR(Testbench) : sorter("sorter", 4) {
        // Connect signals to ports
        for (int i = 0; i < 4; ++i) {
            sorter.input[i](input_signals[i]);
            sorter.output[i](output_signals[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize input array
        int arr[] = { 6, 0, 3, 5 };
        for (int i = 0; i < 4; ++i)
            input_signals[i].write(arr[i]);

        wait(1, SC_NS); // Wait for the sorter to process

        // Print sorted array
        std::cout << "Sorted Array: ";
        for (int i = 0; i < 4; ++i)
            std::cout << output_signals[i].read() << " ";
        std::cout << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
