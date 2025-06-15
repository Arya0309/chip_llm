#include "Dut.h"

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */

#ifndef NATIVE_SYSTEMC
    /* === Variable Section === */
	i_A.clk_rst(i_clk, i_rst);
    o_C.clk_rst(i_clk, i_rst);
    /* === Variable Section End === */
#endif

}

void Dut::do_compute() {

    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        /* === Variable Section === */
        i_A.reset();
        o_C.reset();
        /* === Variable Section End === */
#endif
        wait();
    }

    while (true) {

        int A[4][4]; // 4x4 matrix

#ifndef NATIVE_SYSTEMC
        /* === Variable Section === */
        A[0][0] = i_A.get(); // Row 0, Column 0
        A[0][1] = i_A.get(); // Row 0, Column 1
        A[0][2] = i_A.get(); // Row 0, Column 2
        A[0][3] = i_A.get(); // Row 0, Column 3
        A[1][0] = i_A.get(); // Row 1, Column 0
        A[1][1] = i_A.get(); // Row 1, Column 1
        A[1][2] = i_A.get(); // Row 1, Column 2
        A[1][3] = i_A.get(); // Row 1, Column 3
        A[2][0] = i_A.get(); // Row 2, Column 0
        A[2][1] = i_A.get(); // Row 2, Column 1
        A[2][2] = i_A.get(); // Row 2, Column 2
        A[2][3] = i_A.get(); // Row 2, Column 3
        A[3][0] = i_A.get(); // Row 3, Column 0
        A[3][1] = i_A.get(); // Row 3, Column 1
        A[3][2] = i_A.get(); // Row 3, Column 2
        A[3][3] = i_A.get(); // Row 3, Column 3
#else
        A[0][0] = i_A.read(); // Row 0, Column 0
        A[0][1] = i_A.read(); // Row 0, Column 1
        A[0][2] = i_A.read(); // Row 0, Column 2
        A[0][3] = i_A.read(); // Row 0, Column 3
        A[1][0] = i_A.read(); // Row 1, Column 0
        A[1][1] = i_A.read(); // Row 1, Column 1
        A[1][2] = i_A.read(); // Row 1, Column 2
        A[1][3] = i_A.read(); // Row 1, Column 3
        A[2][0] = i_A.read(); // Row 2, Column 0
        A[2][1] = i_A.read(); // Row 2, Column 1
        A[2][2] = i_A.read(); // Row 2, Column 2
        A[2][3] = i_A.read(); // Row 2, Column 3
        A[3][0] = i_A.read(); // Row 3, Column 0
        A[3][1] = i_A.read(); // Row 3, Column 1
        A[3][2] = i_A.read(); // Row 3, Column 2
        A[3][3] = i_A.read(); // Row 3, Column 3
        /* === Variable Section End === */
#endif

        /* === Main Function ===*/
        int R = 4, C = 4; // Rows and Columns of the matrix
        int mat[R][C]; // Declare a 4x4 matrix
        // Copy the input matrix A into mat
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                mat[i][j] = A[i][j];
            }
        }
        int m = R, n = C; // Initialize m and n to the number of rows and columns
        int row = 0, col = 0; 
        int prev, curr; 

        /* row - Starting row index 
        m - ending row index 
        col - starting column index 
        n - ending column index 
        i - iterator */
        while (row < m && col < n) 
        { 
            if (row + 1 == m || 
                col + 1 == n) 
                break; 

            // Store the first element of 
            // next row, this element will 
            // replace first element of current 
            // row 
            prev = mat[row + 1][col]; 

            /* Move elements of first row from 
            the remaining rows */
            for (int i = col; i < n; i++) 
            { 
                curr = mat[row][i]; 
                mat[row][i] = prev; 
                prev = curr; 
            } 
            row++; 

            /* Move elements of last column 
            from the remaining columns */
            for (int i = row; i < m; i++) 
            { 
                curr = mat[i][n-1]; 
                mat[i][n-1] = prev; 
                prev = curr; 
            } 
            n--; 

            /* Move elements of last row from 
            the remaining rows */
            if (row < m) 
            { 
                for (int i = n-1; i >= col; i--) 
                { 
                    curr = mat[m-1][i]; 
                    mat[m-1][i] = prev; 
                    prev = curr; 
                } 
            } 
            m--; 

            /* Move elements of first column from 
            the remaining rows */
            if (col < n) 
            { 
                for (int i = m-1; i >= row; i--) 
                { 
                    curr = mat[i][col]; 
                    mat[i][col] = prev; 
                    prev = curr; 
                } 
            } 
            col++; 
        } 

        // Print rotated matrix 
        for (int i=0; i<R; i++) { 
            for (int j=0; j<C; j++) {
#ifndef NATIVE_SYSTEMC
                o_C.put(mat[i][j]);
#else
                o_C.write(mat[i][j]);
#endif
            }
        } 
    }
}
