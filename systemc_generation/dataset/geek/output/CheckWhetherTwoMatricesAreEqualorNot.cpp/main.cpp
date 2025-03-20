
#include <systemc.h>

// Matrix Comparator Module
SC_MODULE(MatrixComparator) {
    // Input ports for matrices A and B
    sc_in<sc_int<32>> A[4][4];
    sc_in<sc_int<32>> B[4][4];
    
    // Output port indicating if matrices are identical
    sc_out<bool> identical;

    // Process to compare matrices
    void compareMatrices() {
        bool result = true;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (A[i][j].read() != B[i][j].read()) {
                    result = false;
                    break;
                }
            }
            if (!result) break;
        }
        identical.write(result);
    }

    // Constructor to register the process
    SC_CTOR(MatrixComparator) {
        SC_METHOD(compareMatrices);
        sensitive << A[0][0] << A[0][1] << A[0][2] << A[0][3]
                  << A[1][0] << A[1][1] << A[1][2] << A[1][3]
                  << A[2][0] << A[2][1] << A[2][2] << A[2][3]
                  << A[3][0] << A[3][1] << A[3][2] << A[3][3]
                  << B[0][0] << B[0][1] << B[0][2] << B[0][3]
                  << B[1][0] << B[1][1] << B[1][2] << B[1][3]
                  << B[2][0] << B[2][1] << B[2][2] << B[2][3]
                  << B[3][0] << B[3][1] << B[3][2] << B[3][3];
    }
};

// Testbench Module
SC_MODULE(Testbench) {
    // Signals to connect with MatrixComparator
    sc_signal<sc_int<32>> A[4][4];
    sc_signal<sc_int<32>> B[4][4];
    sc_signal<bool> identical;

    // Instance of MatrixComparator
    MatrixComparator comparator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize matrices A and B
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                A[i][j] = i + 1;
                B[i][j] = i + 1;
            }
        }

        // Wait for one delta cycle to let the comparator process
        wait(1, SC_NS);

        // Print the results
        if (identical.read())
            cout << "Matrices are identical" << endl;
        else
            cout << "Matrices are not identical" << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : comparator("comparator") {
        // Connecting signals to the comparator ports
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                comparator.A[i][j](A[i][j]);
                comparator.B[i][j](B[i][j]);
            }
        }
        comparator.identical(identical);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
