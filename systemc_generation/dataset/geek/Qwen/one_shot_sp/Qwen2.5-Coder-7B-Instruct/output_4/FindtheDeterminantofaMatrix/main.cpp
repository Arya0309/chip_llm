
#include <systemc.h>

// Define constants
static const int N = 4;
static const int SIZE = N * N;

// Module that calculates the determinant of a matrix

SC_MODULE(DeterminantCalculator) {
    // Input port for the matrix (flattened as 1D array)
    sc_in<double> matrix[SIZE];
    // Output port for the determinant
    sc_out<double> det;

    SC_CTOR(DeterminantCalculator) {
        // Use an SC_METHOD to perform the calculation
        SC_METHOD(calculate_determinant);
        for (int i = 0; i < SIZE; i++) {
            sensitive << matrix[i];
        }
    }

    // Method that calculates the determinant
    void calculate_determinant() {
        double a[N][N];
        double det = 1.0;

        // Copy the input matrix into the local array
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                a[i][j] = matrix[i * N + j].read();
            }
        }

        for (int i = 0; i < N; i++) {
            int pivot = i;
            for (int j = i + 1; j < N; j++) {
                if (fabs(a[j][i]) > fabs(a[pivot][i])) {
                    pivot = j;
                }
            }
            if (pivot != i) {
                for (int k = i; k < N; k++) {
                    swap(a[i][k], a[pivot][k]);
                }
                det *= -1;
            }
            if (a[i][i] == 0) {
                det = 0;
                break;
            }
            det *= a[i][i];
            for (int j = i + 1; j < N; j++) {
                double factor = a[j][i] / a[i][i];
                for (int k = i + 1; k < N; k++) {
                    a[j][k] -= factor * a[i][k];
                }
            }
        }

        // Write the determinant to the output port
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
