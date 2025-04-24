scss
#include <systemc.h>

// Module that computes the determinant of a square matrix

SC_MODULE(DeterminantCalculator) {
    sc_in<double> matrix[SIZE]; // Input port for the matrix
    sc_out<double> det; // Output port for the determinant

    // Constructor
    SC_CTOR(DeterminantCalculator) {
        // Process to compute the determinant
        SC_METHOD(compute_determinant);
        sensitive << matrix;
    }

    // Method to compute the determinant
    void compute_determinant() {
        int n = SIZE; // Get the size of the matrix
        double det = 1.0; // Initialize the determinant to 1

        // Perform Gauss-Jordan elimination
        for (int i = 0; i < n; i++) {
            // Find the pivot element and its index
            int pivot = i;
            for (int j = i + 1; j < n; j++) {
                if (matrix[j] > matrix[pivot]) {
                    pivot = j;
                }
            }
            // Check if the pivot is not equal to the current index
            if (pivot != i) {
                swap(matrix[i], matrix[pivot]);
                det *= -1;
            }
            // Check if the current diagonal element is 0
            if (matrix[i][i] == 0) {
                det = 0;
            } else {
                det *= matrix[i][i];
                for (int j = i + 1; j < n; j++) {
                    double factor = matrix[j][i] / matrix[i][i];
                    for (int k = i + 1; k < n; k++) {
                        matrix[j][k] -= factor * matrix[i][k];
                    }
                }
            }
        }
        // Write the result to the output port
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
