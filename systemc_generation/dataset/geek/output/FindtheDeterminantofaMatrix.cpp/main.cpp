
#include <systemc.h>

const int MAXN = 105;

SC_MODULE(DeterminantCalculator) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_out<double> det_out;

    SC_CTOR(DeterminantCalculator) {
        SC_METHOD(process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void process() {
        if (rst.read()) {
            // Reset state
            det = 1.0;
            for (int i = 0; i < MAXN; ++i) {
                for (int j = 0; j < MAXN; ++j) {
                    a[i][j] = 0.0;
                }
            }
            det_out.write(0.0);
        } else {
            // Initialize matrix 'a' with some values
            double matrix[4][4] = {{1, 0, 2, -1},
                                 {3, 0, 0, 5},
                                 {2, 1, 4, -3},
                                 {1, 0, 5, 0}};
            int n = 4;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    a[i][j] = matrix[i][j];
                }
            }

            // Compute determinant
            det = 1.0;
            for (int i = 0; i < n; i++) {
                int pivot = i;
                for (int j = i + 1; j < n; j++) {
                    if (fabs(a[j][i]) > fabs(a[pivot][i])) {
                        pivot = j;
                    }
                }
                if (pivot != i) {
                    swap(a[i], a[pivot]);
                    det *= -1;
                }
                if (a[i][i] == 0) {
                    det = 0;
                    break;
                }
                det *= a[i][i];
                for (int j = i + 1; j < n; j++) {
                    double factor = a[j][i] / a[i][i];
                    for (int k = i + 1; k < n; k++) {
                        a[j][k] -= factor * a[i][k];
                    }
                }
            }
            det_out.write(det);
        }
    }

private:
    double det;
    double a[MAXN][MAXN];
};

int sc_main(int argc, char* argv[]) {
    sc_signal<bool> clk;
    sc_signal<bool> rst;
    sc_signal<double> det_out;

    DeterminantCalculator calc("calc");
    calc.clk(clk);
    calc.rst(rst);
    calc.det_out(det_out);

    sc_start(1, SC_NS); // Reset
    rst.write(true);
    sc_start(1, SC_NS); // Deassert reset
    rst.write(false);
    sc_start(1, SC_NS); // Process one cycle

    std::cout << "Determinant = " << det_out.read() << std::endl;

    return 0;
}
