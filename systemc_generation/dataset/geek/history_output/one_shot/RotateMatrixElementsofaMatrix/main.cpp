
#include <systemc.h>

// Module to rotate a matrix
SC_MODULE(MatrixRotator) {
    sc_in<int> m;  // Number of rows
    sc_in<int> n;  // Number of columns
    sc_in<int> mat[R*C];  // Flattened input matrix
    sc_out<int> rotated_mat[R*C];  // Flattened output matrix

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Process to perform matrix rotation
        SC_METHOD(rotate);
        sensitive << m << n;
        for (int i = 0; i < R*C; ++i) {
            sensitive << mat[i];
        }
    }

    // Method to rotate the matrix
    void rotate() {
        int row = 0, col = 0;
        int prev, curr;
        int local_mat[R][C];

        // Copy input matrix to local array
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                local_mat[i][j] = mat[i*C + j].read();
            }
        }

        // Rotate the matrix
        while (row < m.read() && col < n.read()) {
            if (row + 1 == m.read() || col + 1 == n.read())
                break;

            prev = local_mat[row + 1][col];
            for (int i = col; i < n.read(); i++) {
                curr = local_mat[row][i];
                local_mat[row][i] = prev;
                prev = curr;
            }
            row++;

            for (int i = row; i < m.read(); i++) {
                curr = local_mat[i][n.read()-1];
                local_mat[i][n.read()-1] = prev;
                prev = curr;
            }
            n.write(n.read() - 1);

            if (row < m.read()) {
                for (int i = n.read()-1; i >= col; i--) {
                    curr = local_mat[m.read()-1][i];
                    local_mat[m.read()-1][i] = prev;
                    prev = curr;
                }
            }
            m.write(m.read() - 1);

            if (col < n.read()) {
                for (int i = m.read()-1; i >= row; i--) {
                    curr = local_mat[i][col];
                    local_mat[i][col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Write rotated matrix to output
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                rotated_mat[i*C + j].write(local_mat[i][j]);
            }
        }

        // Print rotated matrix
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                cout << rotated_mat[i*C + j].read() << " ";
            }
            cout << endl;
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> m;  // Number of rows
    sc_signal<int> n;  // Number of columns
    sc_signal<int> mat[R*C];  // Flattened input matrix
    sc_signal<int> rotated_mat[R*C];  // Flattened output matrix

    MatrixRotator matrix_rotator_inst;  // Instance of the MatrixRotator module

    // Constructor
    SC_CTOR(Testbench) : matrix_rotator_inst("matrix_rotator") {
        // Connect signals to ports
        matrix_rotator_inst.m(m);
        matrix_rotator_inst.n(n);
        for (int i = 0; i < R*C; ++i) {
            matrix_rotator_inst.mat[i](mat[i]);
            matrix_rotator_inst.rotated_mat[i](rotated_mat[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Define a test matrix
        int test_matrix[R][C] = {{1, 2, 3, 4},
                                 {5, 6, 7, 8},
                                 {9, 10, 11, 12},
                                 {13, 14, 15, 16}};

        // Set matrix dimensions
        m.write(R);
        n.write(C);

        // Load test matrix into signals
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                mat[i*C + j].write(test_matrix[i][j]);
            }
        }

        // Wait for the matrix rotator to process
        wait(1, SC_NS);

        // Simulation ends after processing
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");  // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
