
#include <systemc.h>

// Module that computes the determinant of a square matrix using Gauss-Jordan elimination

SC_MODULE(DeterminantCalculator) {
    sc_in<int> n;  // Input port for the size of the matrix
    sc_in<double> matrix[MAXN][MAXN];  // Input port for the matrix
    sc_out<double> det;  // Output port for the determinant

    // Method to find the pivot element and swap rows if necessary
    void swap_rows();

    // Method to calculate the determinant using Gauss-Jordan elimination
    void calculate_determinant();

    // Method to print the determinant to the standard output
    void print_det();

    // Constructor
    SC_CTOR(DeterminantCalculator) {
        // Process to swap rows if necessary and calculate the determinant
        SC_METHOD(swap_rows);
        sensitive << n << matrix;
        SC_METHOD(calculate_determinant);
        sensitive << det;

        // Process to print the determinant to the standard output
        SC_METHOD(print_det);
        sensitive << det;
    }
};

// swap_rows method
void DeterminantCalculator::swap_rows() {
    int pivot = n;
    for (int i = 0; i < n; i++) {
        if (abs(matrix[i][pivot]) > abs(matrix[pivot][pivot])) {
            pivot = i;
        }
    }
    if (pivot != n) {
        swap(matrix[n], matrix[pivot]);
        det *= -1;
    }
}

// calculate_determinant method
void DeterminantCalculator::calculate_determinant() {
    det = 1.0;
    for (int i = 0; i < n; i++) {
        if (a[i][i] == 0) {
            return 0;
        }
        det *= a[i][i];
        for (int j = i + 1; j < n; j++) {
            double factor = a[j][i] / a[i][i];
            for (int k = i + 1; k < n; k++) {
                matrix[j][k] -= factor * matrix[i][k];
            }
        }
    }
}

// print_det method
void DeterminantCalculator::print_det() {
    cout << "DeterminantCalculator = " << det << endl;
}


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
