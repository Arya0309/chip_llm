
#include <systemc.h>

// Define a SystemC module for sorting a 2D matrix row-wise
SC_MODULE(MatrixSorter) {
    // Input ports
    sc_in<int> rows;          // Number of rows
    sc_in<int> cols;          // Number of columns
    sc_in<sc_uint<16>> mat_in[4][4]; // Input matrix (fixed size for simplicity)

    // Output ports
    sc_out<sc_uint<16>> mat_out[4][4]; // Sorted matrix

    // Constructor
    SC_CTOR(MatrixSorter) {
        // Process to sort the matrix
        SC_METHOD(sort_matrix);
        sensitive << rows << cols;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                sensitive << mat_in[i][j];
            }
        }
    }

    // Method to sort the matrix row-wise
    void sort_matrix() {
        int r = rows.read();
        int c = cols.read();
        int temp_mat[4][4];

        // Copy input matrix to local array
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                temp_mat[i][j] = mat_in[i][j].read();
            }
        }

        // Perform bubble sort on each row
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                for (int k = 0; k < c - j - 1; k++) {
                    if (temp_mat[i][k] > temp_mat[i][k + 1]) {
                        // Swap elements
                        int temp = temp_mat[i][k];
                        temp_mat[i][k] = temp_mat[i][k + 1];
                        temp_mat[i][k + 1] = temp;
                    }
                }
            }
        }

        // Write sorted matrix to output ports
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                mat_out[i][j].write(temp_mat[i][j]);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> rows;          // Signal for number of rows
    sc_signal<int> cols;          // Signal for number of columns
    sc_signal<sc_uint<16>> mat_in[4][4]; // Signal for input matrix
    sc_signal<sc_uint<16>> mat_out[4][4]; // Signal for output matrix

    // Instance of the MatrixSorter module
    MatrixSorter sorter_inst;

    // Constructor
    SC_CTOR(Testbench) : sorter_inst("sorter") {
        // Connect signals to ports
        sorter_inst.rows(rows);
        sorter_inst.cols(cols);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                sorter_inst.mat_in[i][j](mat_in[i][j]);
                sorter_inst.mat_out[i][j](mat_out[i][j]);
            }
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrix
        int init_mat[4][4] = {{9, 8, 7, 1},
                              {7, 3, 0, 2},
                              {9, 5, 3, 2},
                              {6, 3, 1, 2}};

        // Set matrix dimensions
        rows.write(4);
        cols.write(4);

        // Set matrix values
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat_in[i][j].write(init_mat[i][j]);
            }
        }

        // Wait for the sorter to process
        wait(1, SC_NS);

        // Print sorted matrix
        cout << "Sorted Matrix:" << endl;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cout << mat_out[i][j].read() << " ";
            }
            cout << endl;
        }

        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
