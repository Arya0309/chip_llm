
#include <systemc.h>

// Module to find minimum and maximum elements in an array
SC_MODULE(MinMaxFinder) {
    // Input port for the array size
    sc_in<int> n;

    // Input port for the array elements
    sc_vector<sc_in<int>> arr;

    // Output ports for minimum and maximum elements
    sc_out<int> min_elem;
    sc_out<int> max_elem;

    // Internal variables to store minimum and maximum
    int min_val, max_val;

    // Process to find minimum and maximum elements
    void findMinMax() {
        // Initialize min and max with the first element of the array
        min_val = arr[0].read();
        max_val = arr[0].read();

        // Iterate through the array to find min and max
        for (int i = 1; i < n.read(); ++i) {
            int current = arr[i].read();
            if (current < min_val) min_val = current;
            if (current > max_val) max_val = current;
        }

        // Write the results to the output ports
        min_elem.write(min_val);
        max_elem.write(max_val);
    }

    // Constructor to register the process
    SC_CTOR(MinMaxFinder, const char* name) : sc_module(name), arr("arr", 5) {
        SC_METHOD(findMinMax);
        sensitive << n;
        for (int i = 0; i < arr.size(); ++i) {
            sensitive << arr[i];
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MinMaxFinder
    sc_signal<int> n;
    sc_signal<int> arr[5];
    sc_signal<int> min_elem, max_elem;

    // Instance of MinMaxFinder
    MinMaxFinder minmax_finder;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing array elements
        arr[0] = 12;
        arr[1] = 1234;
        arr[2] = 45;
        arr[3] = 67;
        arr[4] = 1;

        // Set the size of the array
        n = 5;

        // Wait for one delta cycle to let the minmax_finder process
        wait(1, SC_NS);

        // Print the results
        cout << "Minimum element of array: " << min_elem.read() << " ";
        cout << "Maximum element of array: " << max_elem.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : minmax_finder("minmax_finder") {
        // Connecting signals to the minmax_finder ports
        minmax_finder.n(n);
        for (int i = 0; i < 5; ++i) {
            minmax_finder.arr[i](arr[i]);
        }
        minmax_finder.min_elem(min_elem);
        minmax_finder.max_elem(max_elem);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
