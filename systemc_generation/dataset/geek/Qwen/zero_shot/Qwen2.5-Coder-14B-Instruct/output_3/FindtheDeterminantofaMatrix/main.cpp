
#include <systemc.h>


SC_MODULE(DeterminantCalculator) {
    sc_in<double> matrix[SIZE];
    sc_out<double> det;

    void compute() {
        double a[MAXN][MAXN];
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                a[i][j] = matrix[i](j);
            }
        }

        double determinant() {
            double det = 1.0;
            for (int i = 0; i < SIZE; i++) {
                int pivot = i;
                for (int j = i + 1; j < SIZE; j++) {
                    if (abs(a[j][i]) > abs(a[pivot][i])) {
                        pivot = j;
                    }
                }
                if (pivot != i) {
                    swap(a[i], a[pivot]);
                    det *= -1;
                }
                if (a[i][i] == 0) {
                    return 0;
                }
                det *= a[i][i];
                for (int j = i + 1; j < SIZE; j++) {
                    double factor = a[j][i] / a[i][i];
                    for (int k = i + 1; k < SIZE; k++) {
                        a[j][k] -= factor * a[i][k];
                    }
                }
            }
            return det;
        }

        det.write(determinant());
    }

    SC_CTOR(DeterminantCalculator) {
        SC_THREAD(compute);
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
