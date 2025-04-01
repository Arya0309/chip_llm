
#include <systemc.h>

// Module to calculate the average of an array
SC_MODULE(AverageCalculator) {
    sc_in<int> arr[9];  // Input ports for array elements
    sc_in<int> n;       // Input port for the size of the array
    sc_out<double> avg; // Output port for the average

    // Constructor
    SC_CTOR(AverageCalculator) {
        // Process to calculate the average
        SC_METHOD(calculate_average);
        sensitive << n; // Sensitive to changes in the size of the array
        for (int i = 0; i < 9; ++i) {
            sensitive << arr[i]; // Sensitive to changes in each array element
        }
    }

    // Method to calculate the average
    void calculate_average() {
        int sum = 0;
        for (int i = 0; i < n.read(); i++) {
            sum += arr[i].read();
        }
        avg.write((double)sum / n.read());
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> arr[9]; // Signals for array elements
    sc_signal<int> n;      // Signal for the size of the array
    sc_signal<double> avg; // Signal for the average

    AverageCalculator avg_calc_inst; // Instance of the AverageCalculator module

    // Constructor
    SC_CTOR(Testbench) : avg_calc_inst("avg_calc") {
        // Connect signals to ports
        for (int i = 0; i < 9; ++i) {
            avg_calc_inst.arr[i](arr[i]);
        }
        avg_calc_inst.n(n);
        avg_calc_inst.avg(avg);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize array elements
        for (int i = 0; i < 9; ++i) {
            arr[i] = i == 0 ? 10 : i + 1; // Set array elements to {10, 2, 3, ..., 9}
        }
        n = 9; // Set the size of the array

        wait(1, SC_NS); // Wait for the calculator to process
        cout << "Average: " << avg.read() << endl; // Print the calculated average

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
