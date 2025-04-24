
#include <systemc.h>


SC_MODULE(DeterminantCalculator) {
    sc_in<double> matrix[SIZE];  // Input port for the matrix
    sc_out<double> det;          // Output port for the determinant

    double a[SIZE][SIZE];
    int n;

    // Constructor
    SC_CTOR(DeterminantCalculator) {
        n = SIZE;  // Assuming SIZE is defined somewhere in the top-level module
        // Process to calculate the determinant
        SC_METHOD(calculate_determinant);
        sensitive << matrix;
    }

    // Method to calculate the determinant
    void calculate_determinant() {
        double det_val = 1.0;
        for (int i = 0; i < n; i++) {
            int pivot = i;
            for (int j = i + 1; j < n; j++) {
                if (fabs(matrix[j][i].read()) > fabs(matrix[pivot][i].read())) {
                    pivot = j;
                }
            }
            if (pivot != i) {
                for (int k = 0; k < n; k++) {
                    double temp = matrix[i][k].read();
                    matrix[i][k] = matrix[pivot][k].read();
                    matrix[pivot][k] = temp;
                }
                det_val *= -1;
            }
            if (matrix[i][i].read() == 0) {
                det.write(0);
                return;
            }
            det_val *= matrix[i][i].read();
            for (int j = i + 1; j < n; j++) {
                double factor = matrix[j][i].read() / matrix[i][i].read();
                for (int k = i + 1; k < n; k++) {
                    matrix[j][k] = matrix[j][k].read() - factor * matrix[i][k].read();
                }
            }
        }
        det.write(det_val);
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
