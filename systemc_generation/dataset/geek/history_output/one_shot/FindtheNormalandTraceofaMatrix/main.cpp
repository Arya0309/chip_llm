
#include <systemc.h>

// Module to calculate trace and normal of a matrix
SC_MODULE(MatrixOperations) {
    sc_in<sc_matrix<int>> matrix;  // Input port for the matrix
    sc_in<int> size;                // Input port for the size of the matrix
    sc_out<int> trace;             // Output port for the trace
    sc_out<double> normal;          // Output port for the normal

    // Constructor
    SC_CTOR(MatrixOperations) {
        // Process to calculate trace and normal
        SC_METHOD(calculate);
        sensitive << matrix << size;
    }

    // Method to calculate trace and normal
    void calculate() {
        int n = size.read();
        int trace_sum = 0;
        double normal_sum = 0.0;

        // Calculate trace
        for (int i = 0; i < n; i++)
            trace_sum += matrix.read()[i][i];

        // Calculate normal
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                normal_sum += matrix.read()[i][j] * matrix.read()[i][j];

        trace.write(trace_sum);
        normal.write(sqrt(normal_sum));
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<sc_matrix<int>> matrix; // Signal for the matrix
    sc_signal<int> size;              // Signal for the size of the matrix
    sc_signal<int> trace;             // Signal for the trace
    sc_signal<double> normal;          // Signal for the normal

    MatrixOperations matrix_ops_inst; // Instance of the MatrixOperations module

    // Constructor
    SC_CTOR(Testbench) : matrix_ops_inst("matrix_ops") {
        // Connect signals to ports
        matrix_ops_inst.matrix(matrix);
        matrix_ops_inst.size(size);
        matrix_ops_inst.trace(trace);
        matrix_ops_inst.normal(normal);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Define a 5x5 matrix
        sc_matrix<int> mat(5, sc_vector<int>(5));
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
                mat[i][j] = i + 1;

        // Assign matrix and size
        matrix.write(mat);
        size.write(5);

        // Wait for the calculation to complete
        wait(1, SC_NS);

        // Print results
        cout << "Trace of Matrix = " << trace.read() << endl;
        cout << "Normal of Matrix = " << normal.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

// Custom data type for matrix
typedef sc_vector<sc_vector<int>> sc_matrix;

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
