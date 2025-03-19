
#include <systemc.h>

SC_MODULE(MatrixSwapper) {
    SC_CTOR(MatrixSwapper) {
        SC_THREAD(swap_and_print);
    }

    void swap_and_print() {
        const int n = 4;
        int m[n][n] = {{8, 9, 7, 6},
                       {4, 7, 6, 5},
                       {3, 2, 1, 8},
                       {9, 9, 7, 7}};
        
        // Swapping the first and last rows
        for (int i = 0; i < n; ++i) {
            int t = m[0][i];
            m[0][i] = m[n - 1][i];
            m[n - 1][i] = t;
        }
        
        // Printing the modified matrix
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j)
                cout << m[i][j] << " ";
            cout << endl;
        }
    }
};

int sc_main(int argc, char* argv[]) {
    MatrixSwapper matrix_swapper("matrix_swapper");
    sc_start();
    return 0;
}
