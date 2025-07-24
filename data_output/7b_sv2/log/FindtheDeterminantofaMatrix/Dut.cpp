
#include "Dut.h"

constexpr int N = 4;

// Function to calculate the determinant of a matrix
double determinant(double a[N][N], int n) {
    double det = 1.0;
    for (int i = 0; i < n; i++) {
        int pivot = i;
        for (int j = i + 1; j < n; j++) {
            if (std::abs(a[j][i]) > std::abs(a[pivot][i])) {
                pivot = j;
            }
        }
        if (pivot != i) {
            for (int k = 0; k < n; k++) {
                std::swap(a[i][k], a[pivot][k]);
            }
            det *= -1;
        }
        if (a[i][i] == 0) {
            return 0;
        }
        det *= a[i][i];
        for (int j = i + 1; j < n; j++) {
            double factor = a[j][i] / a[i][i];
            for (int k = i + 1; k < n; k++) {
                a[j][k] -= factor * a[i][k];
            }
        }
    }
    return det;
}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        double a[N][N];
        // Read the matrix element-by-element
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                a[i][j] = i_a.read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        int n = N;
        double det = determinant(a, n);
        /* === Main function Section End === */

        /* === Variable Section === */
        // Write the determinant back out
        o_result.write(det);
        /* === Variable Section End === */
    }
}
