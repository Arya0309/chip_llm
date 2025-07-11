#include "Dut.h"

#include <cmath>
#include <algorithm>

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        double a[4][4];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                a[i][j] = i * 4 + j + 1; // Example initialization
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        double det = 1.0;
        for (int i = 0; i < 4; i++) {
            int pivot = i;
            for (int j = i + 1; j < 4; j++) {
                if (std::abs(a[j][i]) > std::abs(a[pivot][i])) {
                    pivot = j;
                }
            }
            if (pivot != i) {
                std::swap(a[i], a[pivot]);
                det *= -1;
            }
            if (a[i][i] == 0) {
                o_result.write(0);
                break;
            }
            det *= a[i][i];
            for (int j = i + 1; j < 4; j++) {
                double factor = a[j][i] / a[i][i];
                for (int k = i + 1; k < 4; k++) {
                    a[j][k] -= factor * a[i][k];
                }
            }
        }
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(det);
        /* === Variable Section End === */
    }
}
