#include <systemc.h>
#include <cassert>
#include <iostream>

// Module that compares two matrices element-by-element
SC_MODULE(MatrixComparator) {
    // Input ports for matrices A and B (flattened as 1D arrays)
    sc_in<int> A[4*4];
    sc_in<int> B[4*4];
    // Output port: true if matrices are identical, false otherwise
    sc_out<bool> identical;

    SC_CTOR(MatrixComparator) {
        // Combinational process to compare the matrices
        SC_METHOD(do_compare);
        // Make the method sensitive to every element of matrices A and B
        for (int i = 0; i < 4*4; i++) {
            sensitive << A[i] << B[i];
        }
    }

    // Method to check if matrices A and B are identical
    void do_compare() {
        bool same = true;
        for (int i = 0; i < 4*4; i++) {
            if (A[i].read() != B[i].read()) {
                same = false;
                break;
            }
        }
        identical.write(same);
    }
};

// Testbench module
SC_MODULE(Testbench) {

    sc_signal<int> A[4*4];
    sc_signal<int> B[4*4];
    sc_signal<bool> result;

    MatrixComparator* comparator;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixComparator module
        comparator = new MatrixComparator("comparator");
        // Connect signals to the module's ports
        for (int i = 0; i < 4*4; i++) {
            comparator->A[i](A[i]);
            comparator->B[i](B[i]);
        }
        comparator->identical(result);

        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes inputs, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize matrices A and B as defined in the original C++ code:
        // Matrix A:
        //   { 1, 1, 1, 1 },
        //   { 2, 2, 2, 2 },
        //   { 3, 3, 3, 3 },
        //   { 4, 4, 4, 4 }
        // Matrix B: identical to Matrix A.
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int index = row * 4 + col;
                A[index].write(row + 1); // For row 0, write 1; row 1, write 2; etc.
                B[index].write(row + 1);
            }
        }

        // Wait for the signals to propagate and for the comparison to be performed.
        wait(1, SC_NS);

        // Check the result and print the output
        if (result.read() == true) {
            std::cout << "Matrices are identical" << std::endl;
        } else {
            std::cout << "Matrices are not identical" << std::endl;
        }

        // Assert the result is true, as the matrices are identical.
        assert(result.read() == true);
        std::cout << "Test passed successfully" << std::endl;

        sc_stop(); // End the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench
    sc_start();         // Start the simulation
    return 0;
}
