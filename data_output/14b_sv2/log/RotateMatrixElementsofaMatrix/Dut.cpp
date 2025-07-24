
#include "Dut.h"

constexpr int R = 4;
constexpr int C = 4;

void rotatematrix(int m, int n, int mat[R][C]) {
    int row = 0, col = 0;
    int prev, curr;

    while (row < m && col < n) {
        if (row + 1 == m || col + 1 == n)
            break;

        prev = mat[row + 1][col];

        for (int i = col; i < n; i++) {
            curr = mat[row][i];
            mat[row][i] = prev;
            prev = curr;
        }
        row++;

        for (int i = row; i < m; i++) {
            curr = mat[i][n - 1];
            mat[i][n - 1] = prev;
            prev = curr;
        }
        n--;

        if (row < m) {
            for (int i = n - 1; i >= col; i--) {
                curr = mat[m - 1][i];
                mat[m - 1][i] = prev;
                prev = curr;
            }
        }
        m--;

        if (col < n) {
            for (int i = m - 1; i >= row; i--) {
                curr = mat[i][col];
                mat[i][col] = prev;
                prev = curr;
            }
        }
        col++;
    }
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
        int mat[R][C];
        // read matrix element-by-element
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                mat[i][j] = i_mat.read();
            }
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        rotatematrix(R, C, mat);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result matrix back out
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                o_result.write(mat[i][j]);
            }
        }
        /* === Variable Section End === */
    }
}
