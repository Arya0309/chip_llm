
#include <systemc.h>

// Module that computes the determinant of a square matrix

SC_MODULE(DeterminantCalculator) {
    sc_in<double>* a;  // Input port for the matrix
    sc_out<double> det; // Output port for the determinant

    // Constructor
    SC_CTOR(DeterminantCalculator) {
        // Process to compute the determinant
        SC_METHOD(compute_determinant);
        sensitive << a;
    }

    // Method to compute the determinant
    void compute_determinant() {
        // Declare variables
        double det = 1.0;
        double pivot;
        int n = a.size();

        // Loop through each row of the matrix
        for (int i = 0; i < n; i++) {
            // Find the pivot element in the current row
            pivot = a[i][i];
            // Swap the pivot element with the element in the first column if it is not already there
            if (pivot != a[i][0]) {
                swap(a[i][0], a[i][i]);
                det *= -1;
            }
            // Compute the determinant of the submatrix obtained by removing the first row and column of the original matrix
            double sub_det = determinant(n - 1, a);
            // Multiply the determinant of the submatrix by the pivot element
            det *= sub_det;
        }
        // Return the determinant of the matrix
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
