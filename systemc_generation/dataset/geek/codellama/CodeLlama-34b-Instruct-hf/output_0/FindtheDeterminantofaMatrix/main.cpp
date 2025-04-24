
#include <systemc.h>


SC_MODULE(DeterminantCalculator) {
    // Input ports for the matrix
sc_in<double[MAXN][MAXN]> matrix;

// Output port for the determinant
sc_out<double> det;

// Define the matrix and determinant signals
sc_signal<double>[MAXN][MAXN] a;
sc_signal<double> det;

// Constructor
SC_CTOR(DeterminantCalculator) {
    sensitive << matrix;
}

// DeterminantCalculator function
void determinant() {
    det.write(1.0);
    for (int i = 0; i < MAXN; i++) {
        int pivot = i;
        for (int j = i + 1; j < MAXN; j++) {
            if (abs(a[j][i]) > abs(a[pivot][i])) {
                pivot = j;
            }
        }
        if (pivot != i) {
            swaprows(i, pivot);
            det.write(-1.0 * det.read());
        }
        if (a[i][i] == 0) {
            det.write(0.0);
            break;
        }
        det.write(det.read() * a[i][i]);
        for (int j = i + 1; j < MAXN; j++) {
            double factor = a[j][i] / a[i][i];
            for (int k = i + 1; k < MAXN; k++) {
                a[j][k] -= factor * a[i][k];
            }
        }
    }
    det.write(det.read() * a[0][0]);
}

// Swap rows function
void swaprows(int i, int j) {
    for (int k = 0; k < MAXN; k++) {
        double temp = a[i][k];
        a[i][k] = a[j][k];
        a[j][k] = temp;
    }
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
