
#include <systemc.h>

// Module to compute trace and normal of a matrix
SC_MODULE(MatrixOperations) {
    sc_in<sc_matrix<int>> mat; // Input matrix
    sc_in<int> n; // Size of the matrix
    sc_out<int> trace; // Output trace
    sc_out<double> normal; // Output normal

    // Constructor
    SC_CTOR(MatrixOperations) {
        // Process to compute trace and normal
        SC_METHOD(compute_trace_normal);
        sensitive << mat << n;
    }

    // Method to compute trace and normal
    void compute_trace_normal() {
        int trace_sum = 0;
        double normal_sum = 0.0;

        // Iterate over the matrix
        for (int i = 0; i < n.read(); i++) {
            for (int j = 0; j < n.read(); j++) {
                normal_sum += mat.read()[i][j] * mat.read()[i][j];
                if (i == j) {
                    trace_sum += mat.read()[i][j];
                }
            }
        }

        // Write results to output ports
        trace.write(trace_sum);
        normal.write(sqrt(normal_sum));
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<sc_matrix<int>> mat; // Signal for the matrix
    sc_signal<int> n; // Signal for the size of the matrix
    sc_signal<int> trace; // Signal for the trace
    sc_signal<double> normal; // Signal for the normal

    MatrixOperations matrix_ops_inst; // Instance of the MatrixOperations module

    // Constructor
    SC_CTOR(Testbench) : matrix_ops_inst("matrix_ops") {
        // Connect signals to ports
        matrix_ops_inst.mat(mat);
        matrix_ops_inst.n(n);
        matrix_ops_inst.trace(trace);
        matrix_ops_inst.normal(normal);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize a 5x5 matrix
        sc_matrix<int> matrix(5, 5);
        matrix[0] = {1, 1, 1, 1, 1};
        matrix[1] = {2, 2, 2, 2, 2};
        matrix[2] = {3, 3, 3, 3, 3};
        matrix[3] = {4, 4, 4, 4, 4};
        matrix[4] = {5, 5, 5, 5, 5};

        // Set matrix and size
        mat.write(matrix);
        n.write(5);

        // Wait for the computation to complete
        wait(1, SC_NS);

        // Print results
        std::cout << "Trace of Matrix = " << trace.read() << std::endl;
        std::cout << "Normal of Matrix = " << normal.read() << std::endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
