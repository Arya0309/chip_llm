
#include <systemc.h>

const int MAXN = 105;

SC_MODULE(MatrixDeterminant) {
    sc_in<bool> clk;            // Clock signal
    sc_in<bool> reset;          // Reset signal
    sc_out<double> determinant; // Output port for the determinant

    double a[MAXN][MAXN];       // Matrix storage
    int n;                      // Size of the matrix

    SC_CTOR(MatrixDeterminant) {
        SC_METHOD(compute_determinant);
        sensitive << clk.posedge_event();
        dont_initialize();
    }

    void initialize_matrix(const double matrix[][MAXN], int size) {
        n = size;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                a[i][j] = matrix[i][j];
            }
        }
    }

    double compute_determinant() {
        if (reset.read()) {
            determinant.write(0.0);
            return 0.0;
        }

        double det = 1.0;
        for (int i = 0; i < n; i++) {
            int pivot = i;
            for (int j = i + 1; j < n; j++) {
                if (abs(a[j][i]) > abs(a[pivot][i])) {
                    pivot = j;
                }
            }
            if (pivot != i) {
                swap(a[i], a[pivot]);
                det *= -1;
            }
            if (a[i][i] == 0) {
                determinant.write(0.0);
                return 0.0;
            }
            det *= a[i][i];
            for (int j = i + 1; j < n; j++) {
                double factor = a[j][i] / a[i][i];
                for (int k = i + 1; k < n; k++) {
                    a[j][k] -= factor * a[i][k];
                }
            }
        }
        determinant.write(det);
        return det;
    }
};

SC_MODULE(Testbench) {
    sc_clock clk;                     // Clock signal
    sc_signal<bool> reset;           // Reset signal
    sc_signal<double> determinant;    // Signal for the determinant

    MatrixDeterminant matrix_det_inst; // Instance of the MatrixDeterminant module

    SC_CTOR(Testbench) : matrix_det_inst("matrix_det_inst"), clk("clk", 10, SC_NS) {
        // Connect signals to ports
        matrix_det_inst.clk(clk);
        matrix_det_inst.reset(reset);
        matrix_det_inst.determinant(determinant);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Initialize the matrix
        double matrix[4][4] = {{1, 0, 2, -1},
                              {3, 0, 0, 5},
                              {2, 1, 4, -3},
                              {1, 0, 5, 0}};
        
        // Reset the system
        reset.write(true);
        wait(10, SC_NS);
        reset.write(false);
        wait(10, SC_NS);

        // Initialize the matrix in the module
        matrix_det_inst.initialize_matrix(matrix, 4);

        // Wait for the computation to complete
        wait(10, SC_NS);

        // Output the determinant
        cout << "Determinant = " << determinant.read() << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
