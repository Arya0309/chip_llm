
#include <systemc.h>

// Module to sort each row of a 2D matrix
SC_MODULE(MatrixSorter) {
    // Ports for input matrix and dimensions
    sc_in<sc_int<32>> matrix[4][4]; // Assuming a fixed size of 4x4
    sc_in<int> rows, cols;
    sc_out<sc_int<32>> sorted_matrix[4][4];

    // Internal variables to store matrix and its dimensions
    int r, c;
    sc_int<32> mat[4][4];
    sc_int<32> sorted_mat[4][4];

    // Process to sort the matrix row-wise
    void sortMatrix() {
        // Read matrix dimensions
        r = rows.read();
        c = cols.read();

        // Read matrix values into internal variable
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                mat[i][j] = matrix[i][j].read();
            }
        }

        // Sort each row using bubble sort
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                for (int k = 0; k < c - j - 1; k++) {
                    if (mat[i][k] > mat[i][k + 1]) {
                        sc_int<32> temp = mat[i][k];
                        mat[i][k] = mat[i][k + 1];
                        mat[i][k + 1] = temp;
                    }
                }
            }
        }

        // Write sorted matrix to output ports
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                sorted_matrix[i][j].write(mat[i][j]);
            }
        }
    }

    // Constructor to register the process
    SC_CTOR(MatrixSorter) {
        SC_METHOD(sortMatrix);
        sensitive << rows << cols;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                sensitive << matrix[i][j];
            }
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MatrixSorter
    sc_signal<sc_int<32>> matrix[4][4];
    sc_signal<int> rows, cols;
    sc_signal<sc_int<32>> sorted_matrix[4][4];

    // Instance of MatrixSorter
    MatrixSorter sorter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the matrix
        matrix[0][0] = 9; matrix[0][1] = 8; matrix[0][2] = 7; matrix[0][3] = 1;
        matrix[1][0] = 7; matrix[1][1] = 3; matrix[1][2] = 0; matrix[1][3] = 2;
        matrix[2][0] = 9; matrix[2][1] = 5; matrix[2][2] = 3; matrix[2][3] = 2;
        matrix[3][0] = 6; matrix[3][1] = 3; matrix[3][2] = 1; matrix[3][3] = 2;

        // Set dimensions
        rows = 4;
        cols = 4;

        // Wait for one delta cycle to let the sorter process
        wait(1, SC_NS);

        // Print the sorted matrix
        cout << "Sorted Matrix:" << endl;
        for (int i = 0; i < rows.read(); i++) {
            for (int j = 0; j < cols.read(); j++) {
                cout << sorted_matrix[i][j].read() << " ";
            }
            cout << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : sorter("sorter") {
        // Connecting signals to the sorter ports
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                sorter.matrix[i][j](matrix[i][j]);
                sorter.sorted_matrix[i][j](sorted_matrix[i][j]);
            }
        }
        sorter.rows(rows);
        sorter.cols(cols);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
