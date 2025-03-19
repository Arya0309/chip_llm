
#include <systemc.h>

// Define a module to compute trace and normal of a matrix
SC_MODULE(MatrixOperations) {
    // Input port for the matrix size
    sc_in<int> n;

    // Input port for the matrix elements
    sc_in<int> mat[MAX][MAX];

    // Output ports for trace and normal
    sc_out<int> trace;
    sc_out<double> normal;

    // Process to compute trace and normal
    void compute() {
        int trace_val = 0;
        double normal_val = 0.0;

        // Read the matrix size
        int size = n.read();

        // Compute trace
        for (int i = 0; i < size; ++i) {
            trace_val += mat[i][i].read();
        }

        // Compute normal (Frobenius norm)
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                normal_val += mat[i][j].read() * mat[i][j].read();
            }
        }
        normal_val = sqrt(normal_val);

        // Write the results to the output ports
        trace.write(trace_val);
        normal.write(normal_val);
    }

    // Constructor to register the process
    SC_CTOR(MatrixOperations) {
        SC_METHOD(compute);
        sensitive << n;
        for (int i = 0; i < MAX; ++i) {
            for (int j = 0; j < MAX; ++j) {
                sensitive << mat[i][j];
            }
        }
    }
};

// Define a testbench to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with MatrixOperations
    sc_signal<int> n;
    sc_signal<int> mat[MAX][MAX];
    sc_signal<int> trace;
    sc_signal<double> normal;

    // Instance of MatrixOperations
    MatrixOperations matrixOps;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize matrix size
        n = 5;

        // Initialize matrix elements
        int init_mat[5][5] = {{1, 1, 1, 1, 1},
                              {2, 2, 2, 2, 2},
                              {3, 3, 3, 3, 3},
                              {4, 4, 4, 4, 4},
                              {5, 5, 5, 5, 5}};

        // Assign initial matrix values to signals
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                mat[i][j] = init_mat[i][j];
            }
        }

        // Wait for one delta cycle to let the computation process
        wait(1, SC_NS);

        // Print the results
        cout << "Trace of Matrix = " << trace.read() << endl;
        cout << "Normal of Matrix = " << normal.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : matrixOps("matrixOps") {
        // Connecting signals to the matrix operations ports
        matrixOps.n(n);
        for (int i = 0; i < MAX; ++i) {
            for (int j = 0; j < MAX; ++j) {
                matrixOps.mat[i][j](mat[i][j]);
            }
        }
        matrixOps.trace(trace);
        matrixOps.normal(normal);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
