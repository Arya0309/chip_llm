#include <systemc.h>

// Use static const for constants
static const int N = 4;
static const int SIZE = N * N;

SC_MODULE(DeterminantCalculator) {
    // Input port: matrix elements (flattened 1D array)
    sc_in<double> matrix[SIZE];
    // Output port: determinant value
    sc_out<double> det;

    SC_CTOR(DeterminantCalculator) {
        SC_THREAD(calculate_det);
        for (int i = 0; i < SIZE; i++) {
            sensitive << matrix[i];
        }
    }

    // Thread that computes the determinant
    void calculate_det() {
        // Wait for input signals to stabilize
        wait(1, SC_NS);

        // Local 2D array to hold the matrix values
        double a[N][N];
        // Read the flattened matrix into 2D local array
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                a[i][j] = matrix[i * N + j].read();
            }
        }

        double determinant_val = 1.0;
        // Gaussian elimination with partial pivoting
        for (int i = 0; i < N; i++) {
            int pivot = i;
            // Find pivot row for column i
            for (int j = i + 1; j < N; j++) {
                if (std::abs(a[j][i]) > std::abs(a[pivot][i])) {
                    pivot = j;
                }
            }
            // Swap rows if needed and update sign of determinant
            if (pivot != i) {
                for (int k = 0; k < N; k++) {
                    double temp = a[i][k];
                    a[i][k] = a[pivot][k];
                    a[pivot][k] = temp;
                }
                determinant_val *= -1;
            }
            // Check for singularity
            if (a[i][i] == 0) {
                det.write(0);
                wait(SC_ZERO_TIME);
                sc_stop();
                return;
            }
            determinant_val *= a[i][i];
            // Eliminate entries below the pivot
            for (int j = i + 1; j < N; j++) {
                double factor = a[j][i] / a[i][i];
                for (int k = i + 1; k < N; k++) {
                    a[j][k] -= factor * a[i][k];
                }
            }
        }
        // Write the computed determinant to output port
        det.write(determinant_val);
        wait(SC_ZERO_TIME);
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
