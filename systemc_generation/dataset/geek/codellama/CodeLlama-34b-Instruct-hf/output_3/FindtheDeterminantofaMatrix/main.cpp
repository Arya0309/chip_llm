scss
#include <systemc.h>


SC_MODULE(DeterminantCalculator) {
    // Input ports for the matrix and its dimensions
    sc_in<double> matrix[SIZE];
    sc_in<int> n;

    // Output port for the determinant
    sc_out<double> det;

    // Constructor
    SC_CTOR(DeterminantCalculator) {
        // Process to calculate the determinant
        SC_METHOD(calcDet);
        sensitive << matrix << n;
    }

    // Method to calculate the determinant
    void calcDet() {
        // Local variables for the pivot and determinant
        double det = 1.0;
        int pivot;

        // Loop over each row and column
        for (int i = 0; i < n; i++) {
            pivot = i;
            for (int j = i + 1; j < n; j++) {
                // Find the largest element in each column
                if (abs(matrix[j][i]) > abs(matrix[pivot][i])) {
                    pivot = j;
                }
            }
            // Swap rows if necessary and update determinant
            if (pivot != i) {
                swap(matrix[i], matrix[pivot]);
                det *= -1;
            }
            // Check for zero pivot
            if (matrix[i][i] == 0) {
                det = 0;
                break;
            }
            // Update determinant
            det *= matrix[i][i];
            // Update lower triangular part of the matrix
            for (int j = i + 1; j < n; j++) {
                for (int k = i + 1; k < n; k++) {
                    matrix[j][k] -= matrix[i][k] * matrix[j][i] / matrix[i][i];
                }
            }
        }
        // Return determinant
        det.write(det);
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
