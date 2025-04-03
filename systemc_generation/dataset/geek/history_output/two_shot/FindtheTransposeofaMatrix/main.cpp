
#include <systemc.h>

// Define the size of the matrix
const int N = 4;

// Module to compute the transpose of a matrix
SC_MODULE(MatrixTranspose) {
    // Input matrix A
    sc_in<int> A[N][N];
    // Output matrix B
    sc_out<int> B[N][N];

    // Constructor
    SC_CTOR(MatrixTranspose) {
        // Register the method to compute transpose
        SC_METHOD(compute_transpose);
        sensitive << A;
    }

    // Method to compute the transpose
    void compute_transpose() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j].write(A[j][i].read());
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    // Signals for the input matrix A and output matrix B
    sc_signal<int> A[N][N];
    sc_signal<int> B[N][N];

    // Instance of the MatrixTranspose module
    MatrixTranspose transpose_inst;

    // Constructor
    SC_CTOR(Testbench) : transpose_inst("transpose_inst") {
        // Connect signals to module ports
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                transpose_inst.A[i][j](A[i][j]);
                transpose_inst.B[i][j](B[i][j]);
            }
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrix A with predefined values
        A[0][0] = 1; A[0][1] = 1; A[0][2] = 1; A[0][3] = 1;
        A[1][0] = 2; A[1][1] = 2; A[1][2] = 2; A[1][3] = 2;
        A[2][0] = 3; A[2][1] = 3; A[2][2] = 3; A[2][3] = 3;
        A[3][0] = 4; A[3][1] = 4; A[3][2] = 4; A[3][3] = 4;

        // Wait for the process to update
        wait(1, SC_NS);

        // Print the transposed matrix B
        cout << "Result matrix is \n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << " " << B[i][j].read();
            }
            cout << "\n";
        }

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
