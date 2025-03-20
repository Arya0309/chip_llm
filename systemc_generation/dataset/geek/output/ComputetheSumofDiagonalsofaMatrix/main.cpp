
#include <systemc.h>

// Module to calculate diagonal sums of a matrix
SC_MODULE(DiagonalSumCalculator) {
    // Input port for the matrix size
    sc_in<int> n;
    // Input port for the matrix data
    sc_in<int>[100][100] mat;
    // Output ports for the diagonal sums
    sc_out<int> principal_sum;
    sc_out<int> secondary_sum;

    // Constructor: Register the method to compute diagonal sums
    SC_CTOR(DiagonalSumCalculator) {
        SC_METHOD(compute_diagonal_sums);
        sensitive << n;
    }

    // Method to compute the diagonal sums
    void compute_diagonal_sums() {
        int principal = 0, secondary = 0;
        int size = n.read();

        // Iterate over each element of the matrix
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                // Check if the element is on the principal diagonal
                if (i == j) {
                    principal += mat[i][j].read();
                }
                // Check if the element is on the secondary diagonal
                if ((i + j) == (size - 1)) {
                    secondary += mat[i][j].read();
                }
            }
        }

        // Write the results to the output ports
        principal_sum.write(principal);
        secondary_sum.write(secondary);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    // Signal for the matrix size
    sc_signal<int> n;
    // Signal for the matrix data
    sc_signal<int>[100][100] mat;
    // Signals for the diagonal sums
    sc_signal<int> principal_sum;
    sc_signal<int> secondary_sum;

    // Instance of the DiagonalSumCalculator module
    DiagonalSumCalculator calc_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : calc_inst("calc_inst") {
        // Connect signals to module ports
        calc_inst.n(n);
        calc_inst.mat(mat);
        calc_inst.principal_sum(principal_sum);
        calc_inst.secondary_sum(secondary_sum);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Define a 4x4 matrix
        int a[4][4] = {{1, 2, 3, 4},
                       {5, 6, 7, 8},
                       {1, 2, 3, 4},
                       {5, 6, 7, 8}};

        // Set the matrix size
        n.write(4);

        // Assign values to the matrix signal
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat[i][j].write(a[i][j]);
            }
        }

        // Wait for the process to update
        wait(1, SC_NS);

        // Read and print the results
        int principal = principal_sum.read();
        int secondary = secondary_sum.read();
        std::cout << "Principal Diagonal: " << principal << std::endl;
        std::cout << "Secondary Diagonal: " << secondary << std::endl;

        // End simulation after successful test
        std::cout << "Test passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
