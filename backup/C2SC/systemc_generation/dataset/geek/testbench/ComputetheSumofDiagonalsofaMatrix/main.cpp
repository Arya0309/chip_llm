#include <systemc.h>
#include <cassert>
#include <iostream>

// Module that computes the diagonal sums of an N x N matrix (flattened as 1D array)
SC_MODULE(DiagonalSum) {
    // Input port: flattened matrix (1D array)
    sc_in<int> matrix[4*4];
    // Output ports for the diagonal sums
    sc_out<int> principal;
    sc_out<int> secondary;

    SC_CTOR(DiagonalSum) {
        // Combinational process to compute diagonal sums
        SC_METHOD(compute_diagonals);
        // Sensitivity list: all matrix elements
        for (int i = 0; i < 4*4; i++) {
            sensitive << matrix[i];
        }
    }

    // Method that computes the principal and secondary diagonal sums
    void compute_diagonals() {
        int princ_sum = 0;
        int sec_sum = 0;
        // Loop over rows of the matrix
        for (int i = 0; i < 4; i++) {
            // Compute index for principal diagonal element: (i, i)
            princ_sum += matrix[i * 4 + i].read();
            // Compute index for secondary diagonal element: (i, 4-1-i)
            sec_sum += matrix[i * 4 + (4 - 1 - i)].read();
        }
        // Write the computed sums to the output ports
        principal.write(princ_sum);
        secondary.write(sec_sum);
    }
};

// Testbench module for the DiagonalSum module
SC_MODULE(Testbench) {

    // Signals for the flattened matrix and the outputs
    sc_signal<int> matrix[4*4];
    sc_signal<int> principal;
    sc_signal<int> secondary;

    // Instance of the DiagonalSum module
    DiagonalSum* diagSum;

    SC_CTOR(Testbench) {
        // Instantiate the DiagonalSum module
        diagSum = new DiagonalSum("diagSum");
        // Connect the matrix signals and outputs
        for (int i = 0; i < 4*4; i++) {
            diagSum->matrix[i](matrix[i]);
        }
        diagSum->principal(principal);
        diagSum->secondary(secondary);

        // Start the test thread
        SC_THREAD(run_tests);
    }

    // Thread to initialize inputs, check outputs, and assert correctness
    void run_tests() {
        // Initialize the matrix as defined in the original C++ code:
        //  Matrix:
        //      {1, 2, 3, 4},
        //      {5, 6, 7, 8},
        //      {1, 2, 3, 4},
        //      {5, 6, 7, 8}
        int mat[4][4] = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {1, 2, 3, 4},
            {5, 6, 7, 8}
        };

        // Write the matrix values to the flattened signal array
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int index = i * 4 + j;
                matrix[index].write(mat[i][j]);
            }
        }

        // Wait for a small time to let the combinational logic settle
        wait(1, SC_NS);

        // Expected sums computed manually:
        // Principal diagonal: 1 + 6 + 3 + 8 = 18
        // Secondary diagonal: 4 + 7 + 2 + 5 = 18
        int expected_principal = 18;
        int expected_secondary = 18;

        // Read the computed outputs
        int computed_principal = principal.read();
        int computed_secondary = secondary.read();

        // Print the results
        std::cout << "Principal Diagonal Sum: " << computed_principal << std::endl;
        std::cout << "Secondary Diagonal Sum: " << computed_secondary << std::endl;

        // Assert that the computed sums match the expected values
        assert(computed_principal == expected_principal);
        assert(computed_secondary == expected_secondary);
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create the testbench instance
    sc_start();         // Start simulation
    return 0;
}
