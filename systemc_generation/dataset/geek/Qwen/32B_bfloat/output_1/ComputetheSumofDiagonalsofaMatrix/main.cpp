
#include <systemc.h>

#define SIZE 4  // Assuming a fixed size matrix for simplicity


SC_MODULE(DiagonalSum) {
    sc_in<int> matrix[SIZE][SIZE];
    sc_out<int> principal;
    sc_out<int> secondary;

    SC_CTOR(DiagonalSum) {
        SC_METHOD(calculateDiagonalSums);
        sensitive << matrix;
    }

    void calculateDiagonalSums() {
        int principal_sum = 0, secondary_sum = 0;
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                if (i == j) {
                    principal_sum += matrix[i][j].read();
                }
                if ((i + j) == (SIZE - 1)) {
                    secondary_sum += matrix[i][j].read();
                }
            }
        }
        principal.write(principal_sum);
        secondary.write(secondary_sum);
    }
};

// Example top module for simulation

SC_MODULE(TopModule) {
    sc_signal<int> matrix[SIZE][SIZE];
    sc_signal<int> principal;
    sc_signal<int> secondary;

    DiagonalSum calculator;

    SC_CTOR(TopModule) : calculator("calculator") {
        // Connect matrix inputs to calculator
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                calculator.matrix[i][j](matrix[i][j]);
            }
        }
        // Connect outputs from calculator
        calculator.principal(principal);
        calculator.secondary(secondary);

        // Initialize matrix values
        SC_THREAD(initializeMatrix);
    }

    void initializeMatrix() {
        wait(1, SC_NS);  // Wait for some time before initialization
        matrix[0][0] = 1; matrix[0][1] = 2; matrix[0][2] = 3; matrix[0][3] = 4;
        matrix[1][0] = 5; matrix[1][1] = 6; matrix[1][2] = 7; matrix[1][3] = 8;
        matrix[2][0] = 1; matrix[2][1] = 2; matrix[2][2] = 3; matrix[2][3] = 4;
        matrix[3][0] = 5; matrix[3][1] = 6; matrix[3][2] = 7; matrix[3][3] = 8;
        wait(1, SC_NS);  // Wait for some time before reading results
        cout << "Principal Diagonal: " << principal.read() << endl;
        cout << "Secondary Diagonal: " << secondary.read() << endl;
        sc_stop();  // Stop simulation after outputting results
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
