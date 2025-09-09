// Matrix multiplication for fixed‑size 2x2 and 2x3 matrices
// Synthesizable C++ code (no I/O, no STL)

#define R1 2
#define C1 2
#define R2 2
#define C2 3

// Multiply mat1 (R1 x C1) by mat2 (R2 x C2) and store result in rslt (R1 x C2)
void mulMat(const int mat1[R1][C1], const int mat2[R2][C2], int rslt[R1][C2])
{
    for (int i = 0; i < R1; ++i) {
        for (int j = 0; j < C2; ++j) {
            rslt[i][j] = 0;
            for (int k = 0; k < R2; ++k) {
                rslt[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}