
#include <systemc.h>
#include <cmath>
#include <iomanip> // For std::setprecision
#include <sstream> // For std::ostringstream


SC_MODULE(MatrixOperations) {
    sc_in<int> matrix[5][5];  // Input matrix of size 5x5
    sc_out<int> trace;       // Output port for the trace
    sc_out<double> norm;     // Output port for the normal (using double for better precision)

    // Constructor
    SC_CTOR(MatrixOperations) {
        // Process to calculate the trace
        SC_METHOD(calculate_trace);
        sensitive << matrix;

        // Process to calculate the normal
        SC_METHOD(calculate_norm);
        sensitive << matrix;
    }

    // Method to calculate the trace
    void calculate_trace() {
        int sum = 0;
        for (int i = 0; i < 5; i++)
            sum += matrix[i][i];
        trace.write(sum);
    }

    // Method to calculate the normal
    void calculate_norm() {
        double max_value = 0;
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
                if (std::abs(matrix[i][j]) > max_value)
                    max_value = std::abs(matrix[i][j]);

        double sum = 0;
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
                sum += static_cast<double>(matrix[i][j] / max_value) * (matrix[i][j] / max_value);

        // Format the norm value to 2 decimal places
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << std::sqrt(sum);
        norm.write(std::stod(oss.str()));
    }
};


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
