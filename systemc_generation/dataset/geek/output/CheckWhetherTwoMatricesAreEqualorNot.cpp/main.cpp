
#include <systemc.h>

const int N = 4;

SC_MODULE(MatrixComparison) {
    sc_in<bool> clk; // Clock signal
    sc_out<bool> result; // Output result: 1 if matrices are identical, 0 otherwise

    SC_CTOR(MatrixComparison) {
        SC_METHOD(compare_matrices);
        sensitive << clk.pos();
    }

    void compare_matrices() {
        int A[N][N] = {{1, 1, 1, 1},
                       {2, 2, 2, 2},
                       {3, 3, 3, 3},
                       {4, 4, 4, 4}};

        int B[N][N] = {{1, 1, 1, 1},
                       {2, 2, 2, 2},
                       {3, 3, 3, 3},
                       {4, 4, 4, 4}};

        bool identical = true;
        for (int i = 0; i < N && identical; i++) {
            for (int j = 0; j < N && identical; j++) {
                if (A[i][j] != B[i][j]) {
                    identical = false;
                }
            }
        }
        result.write(identical);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS); // Create a clock signal
    sc_signal<bool> result;

    MatrixComparison matrix_cmp("matrix_cmp");
    matrix_cmp.clk(clk);
    matrix_cmp.result(result);

    sc_start(20, SC_NS); // Run simulation for 20 nanoseconds

    if (result.read()) {
        cout << "Matrices are identical" << endl;
    } else {
        cout << "Matrices are not identical" << endl;
    }

    return 0;
}
