
#include <systemc.h>

// Module to check if two arrays are equal
SC_MODULE(ArrayChecker) {
    sc_in<bool> clk;           // Clock signal
    sc_in<bool> reset;         // Reset signal
    sc_in<int> N;              // Size of first array
    sc_in<int> M;              // Size of second array
    sc_in<int> arr1[5];        // First array
    sc_in<int> arr2[5];        // Second array
    sc_out<bool> arrays_equal; // Output indicating if arrays are equal

    // Internal variables
    bool equal_flag;
    int sorted_arr1[5];
    int sorted_arr2[5];

    // Constructor
    SC_CTOR(ArrayChecker) {
        // Sensitivity list for checking arrays
        SC_METHOD(check_arrays);
        sensitive << clk.posedge_event();
        dont_initialize();

        // Sensitivity list for reset
        SC_METHOD(reset_arrays);
        sensitive << reset;
    }

    // Method to reset internal state
    void reset_arrays() {
        if (reset.read()) {
            equal_flag = false;
            for (int i = 0; i < 5; ++i) {
                sorted_arr1[i] = 0;
                sorted_arr2[i] = 0;
            }
        }
    }

    // Method to check if arrays are equal
    void check_arrays() {
        if (!reset.read()) {
            // Copy input arrays to internal arrays
            for (int i = 0; i < N.read(); ++i) {
                sorted_arr1[i] = arr1[i].read();
                sorted_arr2[i] = arr2[i].read();
            }

            // Sort the arrays
            sort(sorted_arr1, sorted_arr1 + N.read());
            sort(sorted_arr2, sorted_arr2 + M.read());

            // Check if arrays are equal
            equal_flag = (N.read() == M.read());
            for (int i = 0; i < N.read() && equal_flag; ++i) {
                if (sorted_arr1[i] != sorted_arr2[i]) {
                    equal_flag = false;
                }
            }

            // Write the result to the output
            arrays_equal.write(equal_flag);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;              // Clock signal
    sc_signal<bool> reset;     // Reset signal
    sc_signal<int> N;          // Size of first array
    sc_signal<int> M;          // Size of second array
    sc_signal<int> arr1[5];    // First array
    sc_signal<int> arr2[5];    // Second array
    sc_signal<bool> arrays_equal; // Output indicating if arrays are equal

    ArrayChecker checker_inst; // Instance of the ArrayChecker module

    // Constructor
    SC_CTOR(Testbench) : checker_inst("checker"), clk("clk", 10, SC_NS, 0.5) {
        // Connect signals to ports
        checker_inst.clk(clk);
        checker_inst.reset(reset);
        checker_inst.N(N);
        checker_inst.M(M);
        for (int i = 0; i < 5; ++i) {
            checker_inst.arr1[i](arr1[i]);
            checker_inst.arr2[i](arr2[i]);
        }
        checker_inst.arrays_equal(arrays_equal);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize arrays
        reset = true;
        wait(1, SC_NS);
        reset = false;

        // Test case 1: Arrays are equal
        N = 5; M = 5;
        arr1[0] = 1; arr1[1] = 2; arr1[2] = 3; arr1[3] = 4; arr1[4] = 5;
        arr2[0] = 5; arr2[1] = 4; arr2[2] = 3; arr2[3] = 2; arr2[4] = 1;
        wait(1, SC_NS);
        assert(arrays_equal.read() == true);
        cout << "Test 1: Arrays are " << (arrays_equal.read() ? "Equal" : "Not Equal") << endl;

        // Test case 2: Arrays are not equal
        N = 5; M = 5;
        arr1[0] = 1; arr1[1] = 2; arr1[2] = 3; arr1[3] = 4; arr1[4] = 5;
        arr2[0] = 1; arr2[1] = 2; arr2[2] = 3; arr2[3] = 4; arr2[4] = 6;
        wait(1, SC_NS);
        assert(arrays_equal.read() == false);
        cout << "Test 2: Arrays are " << (arrays_equal.read() ? "Equal" : "Not Equal") << endl;

        // Test case 3: Arrays of different sizes
        N = 4; M = 5;
        arr1[0] = 1; arr1[1] = 2; arr1[2] = 3; arr1[3] = 4;
        arr2[0] = 1; arr2[1] = 2; arr2[2] = 3; arr2[3] = 4; arr2[4] = 5;
        wait(1, SC_NS);
        assert(arrays_equal.read() == false);
        cout << "Test 3: Arrays are " << (arrays_equal.read() ? "Equal" : "Not Equal") << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
