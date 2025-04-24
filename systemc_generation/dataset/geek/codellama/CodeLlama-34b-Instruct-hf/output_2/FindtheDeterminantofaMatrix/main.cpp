
#include <systemc.h>

// Module that computes the determinant of a matrix

SC_MODULE(DeterminantCalculator) {
    sc_in<int> n; // Input port for the number of rows and columns
    sc_in<double[n][n]> a; // Input port for the matrix to be processed
    sc_out<double> det; // Output port for the determinant

    // Constructor
    SC_CTOR(DeterminantCalculator) {
        // Process to compute the determinant
        SC_METHOD(computeDet);
        sensitive << n << a;
    }

    // Method to compute the determinant
    void computeDet() {
        // Initialize the determinant to 1.0
        det.write(1.0);

        // Loop through each row of the matrix
        for (int i = 0; i < n.read(); i++) {
            // Check if the diagonal element is not zero
            if (a.read()[i][i] == 0.0) {
                det.write(0.0);
                return;
            }

            // Find the pivot element and flip the sign of the determinant if necessary
            int pivot = i;
            for (int j = i + 1; j < n.read(); j++) {
                if (abs(a.read()[j][i]) > abs(a.read()[pivot][i])) {
                    pivot = j;
                }
            }
            if (pivot != i) {
                det.write(-det.read());
                swap(a.read()[i], a.read()[pivot]);
            }

            // Perform Gaussian elimination on the current row
            double factor = a.read()[i][i];
            for (int j = i + 1; j < n.read(); j++) {
                a.write()[j][i] -= factor * a.read()[i][j];
            }
        }

        // Compute the determinant of the upper triangular matrix
        det.write(det.read() * a.read()[n.read() - 1][n.read() - 1]);
    }
};


SC_MODULE(Testbench) {
    // Use static const for constants
    static const int N = 4;
    static const int SIZE = N * N;
    
    sc_signal<double> matrix[SIZE];
    sc_signal<double> det;

    DeterminantCalculator* detCalc;

    SC_CTOR(Testbench) {
        detCalc = new DeterminantCalculator("detCalc");
        for (int i = 0; i < SIZE; i++) {
            detCalc->matrix[i](matrix[i]);
        }
        detCalc->det(det);
        SC_THREAD(run_tests);
    }

    // Thread to initialize matrix, check result, and assert correctness
    void run_tests() {
        // Initialize the 4x4 matrix (row-major order)
        // Matrix:
        // { {1, 0, 2, -1},
        //   {3, 0, 0, 5},
        //   {2, 1, 4, -3},
        //   {1, 0, 5, 0} }
        double init_matrix[SIZE] = {
            1, 0, 2, -1,
            3, 0, 0, 5,
            2, 1, 4, -3,
            1, 0, 5, 0
        };
        for (int i = 0; i < SIZE; i++) {
            matrix[i].write(init_matrix[i]);
        }

        // Wait for signals to propagate and process to complete
        wait(2, SC_NS);

        // Read the result and check
        double result = det.read();
        std::cout << "Determinant = " << result << std::endl;
        // Expected determinant is 30 (as computed in the original code)
        assert(std::abs(result - 30.0) < 1e-6);
        std::cout << "Test passed successfully" << std::endl;

        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
