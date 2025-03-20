
#include <systemc.h>

// Module to perform matrix transpose
SC_MODULE(MatrixTranspose) {
    // Input matrix A
    sc_in<int> A[N*N];

    // Output matrix B
    sc_out<int> B[N*N];

    // Process to compute the transpose of matrix A and store it in matrix B
    void computeTranspose() {
        int temp[N][N];
        
        // Copy input matrix A into a 2D array for easier manipulation
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                temp[i][j] = A[i * N + j].read();

        // Compute transpose
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                B[j * N + i].write(temp[i][j]);
    }

    // Constructor to register the process
    SC_CTOR(MatrixTranspose) {
        SC_METHOD(computeTranspose);
        sensitive << A;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MatrixTranspose
    sc_signal<int> A[N*N];
    sc_signal<int> B[N*N];

    // Instance of MatrixTranspose
    MatrixTranspose transposeModule;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize matrix A with predefined values
        int initA[N][N] = {{1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3}, {4, 4, 4, 4}};
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                A[i * N + j] = initA[i][j];

        // Wait for one delta cycle to let the transposeModule process
        wait(1, SC_NS);

        // Print the result matrix B
        cout << "Result matrix is \n";
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j)
                cout << " " << B[i * N + j].read();
            cout << "\n";
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : transposeModule("transposeModule") {
        // Connecting signals to the transposeModule ports
        for (int i = 0; i < N * N; ++i) {
            transposeModule.A[i](A[i]);
            transposeModule.B[i](B[i]);
        }

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
