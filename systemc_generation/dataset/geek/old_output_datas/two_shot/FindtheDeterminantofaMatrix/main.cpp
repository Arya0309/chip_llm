
#include <systemc.h>

// Module to calculate the determinant of a matrix
SC_MODULE(MatrixDeterminant) {
    // Input port for the matrix size
    sc_in<int> n;
    // Input port for the matrix elements
    sc_in<double> matrix[105][105];
    // Output port for the determinant
    sc_out<double> determinant;

    // Internal storage for the matrix
    double a[105][105];

    // Constructor: Register the method to compute determinant
    SC_CTOR(MatrixDeterminant) {
        SC_METHOD(compute_determinant);
        sensitive << n;
        for (int i = 0; i < 105; ++i) {
            for (int j = 0; j < 105; ++j) {
                sensitive << matrix[i][j];
            }
        }
    }

    // Method to compute the determinant
    void compute_determinant() {
        int size = n.read();
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                a[i][j] = matrix[i][j].read();
            }
        }

        double det = 1.0;
        for (int i = 0; i < size; i++) {
            int pivot = i;
            for (int j = i + 1; j < size; j++) {
                if (fabs(a[j][i]) > fabs(a[pivot][i])) {
                    pivot = j;
                }
            }
            if (pivot != i) {
                std::swap(a[i], a[pivot]);
                det *= -1;
            }
            if (a[i][i] == 0) {
                determinant.write(0);
                return;
            }
            det *= a[i][i];
            for (int j = i + 1; j < size; j++) {
                double factor = a[j][i] / a[i][i];
                for (int k = i + 1; k < size; k++) {
                    a[j][k] -= factor * a[i][k];
                }
            }
        }
        determinant.write(det);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n_sig;                     // Signal for matrix size
    sc_signal<double> matrix_sig[105][105];    // Signal for matrix elements
    sc_signal<double> det_sig;                 // Signal for determinant output

    MatrixDeterminant md_inst;                  // Instance of the MatrixDeterminant module

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : md_inst("md_inst") {
        // Connect signals to module ports
        md_inst.n(n_sig);
        for (int i = 0; i < 105; ++i) {
            for (int j = 0; j < 105; ++j) {
                md_inst.matrix[i][j](matrix_sig[i][j]);
            }
        }
        md_inst.determinant(det_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set matrix size
        n_sig.write(4);
        // Set matrix elements
        matrix_sig[0][0].write(1); matrix_sig[0][1].write(0); matrix_sig[0][2].write(2); matrix_sig[0][3].write(-1);
        matrix_sig[1][0].write(3); matrix_sig[1][1].write(0); matrix_sig[1][2].write(0); matrix_sig[1][3].write(5);
        matrix_sig[2][0].write(2); matrix_sig[2][1].write(1); matrix_sig[2][2].write(4); matrix_sig[2][3].write(-3);
        matrix_sig[3][0].write(1); matrix_sig[3][1].write(0); matrix_sig[3][2].write(5); matrix_sig[3][3].write(0);

        // Wait for the process to update
        wait(1, SC_NS);

        // Read and print the determinant
        double result = det_sig.read();
        std::cout << "Determinant = " << result << std::endl;

        // End simulation after successful test
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
