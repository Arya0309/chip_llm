
#include <systemc.h>

SC_MODULE(MatrixSorter) {
    SC_CTOR(MatrixSorter) {
        SC_THREAD(sortRowWise);
    }

    void sortRowWise() {
        int m[4][4] = {{9, 8, 7, 1},
                       {7, 3, 0, 2},
                       {9, 5, 3, 2},
                       {6, 3, 1, 2}};
        int r = 4;
        int c = 4;

        // Sort each row using bubble sort
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                for (int k = 0; k < c - j - 1; k++) {
                    if (m[i][k] > m[i][k + 1]) {
                        // Swap elements
                        int temp = m[i][k];
                        m[i][k] = m[i][k + 1];
                        m[i][k + 1] = temp;
                    }
                }
            }
        }

        // Print sorted matrix
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                cout << m[i][j] << " ";
            }
            cout << endl;
        }
    }
};

int sc_main(int argc, char* argv[]) {
    MatrixSorter sorter("MatrixSorter");
    sc_start();
    return 0;
}
