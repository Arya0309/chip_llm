c
#include <systemc.h>


SC_MODULE(DeterminantCalculator) {
	sc_in<double> matrix[MAXN][MAXN];  // Input matrix
	sc_out<double> det;  // Output determinant

	SC_CTOR(DeterminantCalculator) {
		SC_METHOD(calculate_determinant);
		sensitive << matrix[0][0];
	}

	void calculate_determinant() {
		double det = 1.0;
		for (int i = 0; i < MAXN; i++) {
			int pivot = i;
			for (int j = i + 1; j < MAXN; j++) {
				if (abs(matrix[j][i]) > abs(matrix[pivot][i])) {
					pivot = j;
				}
			}
			if (pivot != i) {
				for (int k = 0; k < MAXN; k++) {
					double tmp = matrix[i][k];
					matrix[i][k] = matrix[pivot][k];
					matrix[pivot][k] = tmp;
				}
				det *= -1;
			}
			if (matrix[i][i] == 0) {
				return;
			}
			det *= matrix[i][i];
			for (int j = i + 1; j < MAXN; j++) {
				double factor = matrix[j][i] / matrix[i][i];
				for (int k = i + 1; k < MAXN; k++) {
					matrix[j][k] -= factor * matrix[i][k];
				}
			}
		}
		det = 0;
		for (int i = 0; i < MAXN; i++) {
			for (int j = 0; j < MAXN; j++) {
				if (i == j) {
					det *= matrix[i][j];
				}
			}
		}
		det = det / matrix[0][0];
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
