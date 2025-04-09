
#include <systemc.h>


SC_MODULE(DiagonalSum) {
    sc_in<int> matrix[SIZE]; // Input signal for the matrix elements
    sc_out<int> principal;    // Output signal for the principal diagonal sum
    sc_out<int> secondary;    // Output signal for the secondary diagonal sum

    // Constructor
    SC_CTOR(DiagonalSum) {
        // Process to calculate diagonal sums
        SC_METHOD(calculateDiagonalSums);
        sensitive << matrix[0] << matrix[1] << matrix[2] << matrix[3]; // Sensitive to all matrix inputs
    }

    // Method to calculate the sums of the principal and secondary diagonals
    void calculateDiagonalSums() {
        int principalSum = 0;
        int secondarySum = 0;
        int n = SIZE; // Assuming SIZE is defined and equals the matrix dimension

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int element = matrix[i * n + j].read(); // Accessing matrix element
                if (i == j) {
                    principalSum += element; // Add to principal diagonal sum
                }
                if ((i + j) == (n - 1)) {
                    secondarySum += element; // Add to secondary diagonal sum
                }
            }
        }

        principal.write(principalSum); // Write principal diagonal sum to output
        secondary.write(secondarySum); // Write secondary diagonal sum to output
    }
};


SC_MODULE(Testbench) {
    // Define constants for matrix dimensions
    static const int N = 4;
    static const int SIZE = N * N;
    // Signals for the flattened matrix and the outputs
    sc_signal<int> matrix[SIZE];
    sc_signal<int> principal;
    sc_signal<int> secondary;

    // Instance of the DiagonalSum module
    DiagonalSum* diagSum;

    SC_CTOR(Testbench) {
        // Instantiate the DiagonalSum module
        diagSum = new DiagonalSum("diagSum");
        // Connect the matrix signals and outputs
        for (int i = 0; i < SIZE; i++) {
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
        int mat[N][N] = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {1, 2, 3, 4},
            {5, 6, 7, 8}
        };

        // Write the matrix values to the flattened signal array
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int index = i * N + j;
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
