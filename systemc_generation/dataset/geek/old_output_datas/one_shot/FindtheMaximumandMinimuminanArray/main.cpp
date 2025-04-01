
#include <systemc.h>
#include <algorithm> // For std::min_element and std::max_element

// Module to find minimum and maximum elements in an array
SC_MODULE(MinMaxFinder) {
    sc_in<sc_int<32>> arr[5];  // Input ports for the array elements
    sc_out<sc_int<32>> min_val; // Output port for the minimum value
    sc_out<sc_int<32>> max_val; // Output port for the maximum value

    // Constructor
    SC_CTOR(MinMaxFinder) {
        // Process to find min and max
        SC_METHOD(find_min_max);
        sensitive << arr[0] << arr[1] << arr[2] << arr[3] << arr[4];
    }

    // Method to find the minimum and maximum values
    void find_min_max() {
        sc_int<32> local_arr[5];
        for (int i = 0; i < 5; ++i) {
            local_arr[i] = arr[i].read();
        }
        min_val.write(*std::min_element(local_arr, local_arr + 5));
        max_val.write(*std::max_element(local_arr, local_arr + 5));
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<sc_int<32>> arr[5]; // Signals for the array elements
    sc_signal<sc_int<32>> min_val; // Signal for the minimum value
    sc_signal<sc_int<32>> max_val; // Signal for the maximum value

    MinMaxFinder min_max_finder_inst; // Instance of the MinMaxFinder module

    // Constructor
    SC_CTOR(Testbench) : min_max_finder_inst("min_max_finder") {
        // Connect signals to ports
        for (int i = 0; i < 5; ++i) {
            min_max_finder_inst.arr[i](arr[i]);
        }
        min_max_finder_inst.min_val(min_val);
        min_max_finder_inst.max_val(max_val);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize the array elements
        arr[0] = 12;
        arr[1] = 1234;
        arr[2] = 45;
        arr[3] = 67;
        arr[4] = 1;
        wait(1, SC_NS); // Wait for the MinMaxFinder to process

        // Output the results
        cout << "Minimum element of array: " << min_val.read() << " ";
        cout << "Maximum element of array: " << max_val.read() << endl;

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
