
#include <systemc.h>

SC_MODULE(MatrixSorter) {
    SC_CTOR(MatrixSorter) {
        SC_THREAD(sort_row_wise);
    }

    void sort_row_wise() {
        int m[4][4] = {{9, 8, 7, 1},
                       {7, 3, 0, 2},
                       {9, 5, 3, 2},
                       {6, 3, 1, 2}};
        int r = 4;
        int c = 4;

        // Sorting logic
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                for (int k = 0; k < c - j - 1; k++) {
                    if (m[i][k] > m[i][k + 1]) {
                        // Swapping of elements
                        int temp = m[i][k];
                        m[i][k] = m[i][k + 1];
                        m[i][k + 1] = temp;
                    }
                }
            }
        }

        // Printing the sorted matrix
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                cout << m[i][j] << " ";
            }
            cout << endl;
        }

        sc_stop(); // Stop the simulation after printing
    }
};

int sc_main(int argc, char* argv[]) {
    MatrixSorter sorter("MatrixSorter");
    sc_start();
    return 0;
}
