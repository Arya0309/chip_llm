
#include <systemc.h>
#include <algorithm> // For std::sort

// Module to check if two arrays are equal
SC_MODULE(ArrayEqualityChecker) {
    // Input ports for the arrays and their sizes
    sc_in<sc_int<32>> arr1[5]; // Assuming fixed size of 5 for simplicity
    sc_in<sc_int<32>> arr2[5];
    sc_in<sc_uint<8>> N; // Size of the first array
    sc_in<sc_uint<8>> M; // Size of the second array

    // Output port for the result
    sc_out<bool> are_equal;

    // Internal storage for sorted arrays
    sc_int<32> sorted_arr1[5];
    sc_int<32> sorted_arr2[5];

    // Constructor: Register the method to check arrays
    SC_CTOR(ArrayEqualityChecker) {
        SC_METHOD(check_arrays);
        sensitive << N << M;
        for (int i = 0; i < 5; ++i) {
            sensitive << arr1[i] << arr2[i];
        }
    }

    // Method to check if arrays are equal
    void check_arrays() {
        // Read sizes
        int n = N.read();
        int m = M.read();

        // Check if sizes are equal
        if (n != m) {
            are_equal.write(false);
            return;
        }

        // Read arrays into internal storage
        for (int i = 0; i < n; ++i) {
            sorted_arr1[i] = arr1[i].read();
            sorted_arr2[i] = arr2[i].read();
        }

        // Sort the arrays
        std::sort(sorted_arr1, sorted_arr1 + n);
        std::sort(sorted_arr2, sorted_arr2 + m);

        // Compare elements
        bool equal = true;
        for (int i = 0; i < n; ++i) {
            if (sorted_arr1[i] != sorted_arr2[i]) {
                equal = false;
                break;
            }
        }

        // Write result
        are_equal.write(equal);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    // Signals for the arrays and their sizes
    sc_signal<sc_int<32>> arr1[5];
    sc_signal<sc_int<32>> arr2[5];
    sc_signal<sc_uint<8>> N;
    sc_signal<sc_uint<8>> M;

    // Signal for the result
    sc_signal<bool> are_equal;

    // Instance of the ArrayEqualityChecker module
    ArrayEqualityChecker checker_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : checker_inst("checker_inst") {
        // Connect signals to module ports
        for (int i = 0; i < 5; ++i) {
            checker_inst.arr1[i](arr1[i]);
            checker_inst.arr2[i](arr2[i]);
        }
        checker_inst.N(N);
        checker_inst.M(M);
        checker_inst.are_equal(are_equal);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize arrays and sizes
        for (int i = 0; i < 5; ++i) {
            arr1[i].write(i + 1);
            arr2[i].write(5 - i);
        }
        N.write(5);
        M.write(5);
        wait(1, SC_NS); // Wait for the process to update
        bool result = are_equal.read();
        std::cout << "Arrays are " << (result ? "Equal" : "Not Equal") << std::endl;

        // End simulation after successful test
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
