
#include <systemc.h>


SC_MODULE(DeterminantCalculator) {
    sc_in<double> matrix[16]; // Assuming a 4x4 matrix for simplicity
    sc_out<double> det;

    SC_CTOR(DeterminantCalculator) {
        SC_METHOD(calculate_determinant);
        sensitive << matrix[0];
    }

    void calculate_determinant() {
        const int n = 4;
        double a[n][n];
        int index = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                a[i][j] = matrix[index].read();
                index++;
            }
        }

        double det_value = 1.0;
        for (int i = 0; i < n; i++) {
            int pivot = i;
            for (int j = i + 1; j < n; j++) {
                if (fabs(a[j][i]) > fabs(a[pivot][i])) {
                    pivot = j;
                }
            }
            if (pivot != i) {
                std::swap(a[i], a[pivot]);
                det_value *= -1;
            }
            if (a[i][i] == 0) {
                det.write(0.0);
                return;
            }
            det_value *= a[i][i];
            for (int j = i + 1; j < n; j++) {
                double factor = a[j][i] / a[i][i];
                for (int k = i + 1; k < n; k++) {
                    a[j][k] -= factor * a[i][k];
                }
            }
        }
        det.write(det_value);
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
