
#include <systemc.h>

#define R1 2
#define C1 2
#define R2 2
#define C2 3

SC_MODULE(MatrixMultiplier) {
    SC_CTOR(MatrixMultiplier) {
        SC_THREAD(multiply_matrices);
    }

    void multiply_matrices() {
        int mat1[R1][C1] = { { 1, 1 }, { 2, 2 } };
        int mat2[R2][C2] = { { 1, 1, 1 }, { 2, 2, 2 } };
        int rslt[R1][C2];

        if (C1 != R2) {
            sc_assert(false && "The number of columns in Matrix-1 must be equal to the number of rows in Matrix-2");
        }

        for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C2; j++) {
                rslt[i][j] = 0;
                for (int k = 0; k < R2; k++) {
                    rslt[i][j] += mat1[i][k] * mat2[k][j];
                }
                cout << rslt[i][j] << "\t";
            }
            cout << endl;
        }
    }
};

int sc_main(int argc, char* argv[]) {
    MatrixMultiplier multiplier("multiplier");
    sc_start();
    return 0;
}
