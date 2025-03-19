
#include <systemc.h>

// Define a module for matrix transpose
SC_MODULE(MatrixTranspose) {
    // Define input and output ports for matrices
    sc_in<int> A[4][4]; // Input matrix
    sc_out<int> B[4][4]; // Output matrix

    SC_CTOR(MatrixTranspose) {
        // Process to perform matrix transpose
        SC_METHOD(transpose);
        sensitive << A[0][0]; // Sensitivity list (can be adjusted)
    }

    void transpose() {
        // Perform the transpose operation
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                B[i][j].write(A[j][i].read());
            }
        }
    }
};

int sc_main(int argc, char* argv[]) {
    // Instantiate the matrix transpose module
    MatrixTranspose mat_trans("mat_trans");

    // Initialize input matrix A
    int A_init[4][4] = {{1, 1, 1, 1},
                        {2, 2, 2, 2},
                        {3, 3, 3, 3},
                        {4, 4, 4, 4}};

    // Create signals for input and output matrices
    sc_signal<int> A_sig[4][4];
    sc_signal<int> B_sig[4][4];

    // Bind input matrix A to signals
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            A_sig[i][j].write(A_init[i][j]);
            mat_trans.A[i][j](A_sig[i][j]);
            mat_trans.B[i][j](B_sig[i][j]);
        }
    }

    // Simulation loop to print the transposed matrix
    sc_start(1, SC_NS); // Start simulation for 1 nanosecond
    cout << "Result matrix is \n";
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << " " << B_sig[i][j].read();
        }
        cout << "\n";
    }

    return 0;
}
