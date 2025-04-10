#include <systemc.h>


// Use static const for constants
static const int N = 5;        // Dimension of the matrix (5x5)
static const int SIZE = N * N; // Total number of matrix elements

// Module that computes the trace and normal (Frobenius norm) of a matrix
SC_MODULE(MatrixOperations) {
    // Input ports: flattened matrix (1D array representing N x N matrix)
    sc_in<int> matrix[SIZE];

    // Output ports for computed trace and normal
    sc_out<int> trace;
    sc_out<int> norm;

    SC_CTOR(MatrixOperations) {
        SC_METHOD(compute);
        // Make the method sensitive to all matrix elements
        for (int i = 0; i < SIZE; i++) {
            sensitive << matrix[i];
        }
    }

    // Combinational method that computes the trace and normal of the matrix
    void compute() {
        int t = 0;       // Variable to hold the trace
        int sum_sq = 0;  // Variable to hold the sum of squares of all elements

        // Loop through each element in the matrix (using 2D indexing on a flattened array)
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                int val = matrix[index].read();
                sum_sq += val * val;  // Add square of the element to the sum
                if (row == col) {      // Check if the element is on the diagonal
                    t += val;
                }
            }
        }
        // Write the computed trace and normal to the output ports
        trace.write(t);
        norm.write(static_cast<int>(sqrt(sum_sq))); // Truncate the result to an integer
    }
};

// Testbench module to drive the matrix signals and verify outputs
SC_MODULE(Testbench) {
    // Use static const for constants
    static const int N = 5;        // Dimension of the matrix (5x5)
    static const int SIZE = N * N; // Total number of matrix elements   
    // Signals to connect with the MatrixOperations module
    sc_signal<int> matrix[SIZE];
    sc_signal<int> trace_signal;
    sc_signal<int> norm_signal;

    // Instance of the MatrixOperations module
    MatrixOperations* matrixOps;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixOperations module
        matrixOps = new MatrixOperations("matrixOps");

        // Connect the matrix signals to the module ports
        for (int i = 0; i < SIZE; i++) {
            matrixOps->matrix[i](matrix[i]);
        }
        matrixOps->trace(trace_signal);
        matrixOps->norm(norm_signal);

        // Create a thread to run test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes the matrix, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize the matrix with values as defined in the original C++ code:
        // Matrix:
        //   {1, 1, 1, 1, 1},
        //   {2, 2, 2, 2, 2},
        //   {3, 3, 3, 3, 3},
        //   {4, 4, 4, 4, 4},
        //   {5, 5, 5, 5, 5}
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                matrix[index].write(row + 1); // Set all elements in a row to row+1
            }
        }

        // Wait for the combinational logic to settle
        wait(1, SC_NS);

        // Expected outputs:
        // Trace = 1 + 2 + 3 + 4 + 5 = 15
        // Normal = sqrt(5*(1^2 + 2^2 + 3^2 + 4^2 + 5^2)) = sqrt(275) ≈ 16 (when truncated)
        int expected_trace = 15;
        int expected_norm = static_cast<int>(sqrt(275));

        std::cout << "Computed Trace: " << trace_signal.read() << std::endl;
        std::cout << "Computed Normal: " << norm_signal.read() << std::endl;

        // Validate the computed outputs using assertions
        assert(trace_signal.read() == expected_trace);
        assert(norm_signal.read() == expected_norm);

        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // End the simulation
    }
};

// sc_main: the entry point of the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench
    sc_start();         // Start the simulation
    return 0;
}
