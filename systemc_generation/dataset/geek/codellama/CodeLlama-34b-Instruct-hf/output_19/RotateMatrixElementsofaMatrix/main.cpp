
#include <systemc.h>

// Module that rotates a matrix by 90 degrees clockwise

SC_MODULE(MatrixRotator) {
    sc_in<int> row; // Input port for the number of rows
    sc_in<int> col; // Input port for the number of columns
    sc_in<sc_matrix<int>> mat; // Input port for the matrix to be rotated
    sc_out<sc_matrix<int>> rot_mat; // Output port for the rotated matrix

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Process to perform rotation
        SC_METHOD(rotate);
        sensitive << row << col;
    }

    // Method to rotate the matrix
    void rotate() {
        sc_matrix<int> rot_mat;
        // Initialize the rotated matrix with zeros
        for (int i = 0; i < row.read(); i++) {
            for (int j = 0; j < col.read(); j++) {
                rot_mat[i][j] = 0;
            }
        }

        int row = 0, col = 0;
        while (row < row.read() && col < col.read()) {
            // Store the first element of the next row
            int prev = mat[row + 1][col];

            // Move elements of first row from remaining rows
            for (int i = col; i < col.read(); i++) {
                int curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }
            row++;

            // Move elements of last column from remaining columns
            for (int i = row; i < row.read(); i++) {
                int curr = mat[i][col.read() - 1];
                mat[i][col.read() - 1] = prev;
                prev = curr;
            }
            col--;

            // Move elements of last row from remaining rows
            if (row < row.read()) {
                for (int i = col.read() - 1; i >= row; i--) {
                    int curr = mat[row.read() - 1][i];
                    mat[row.read() - 1][i] = prev;
                    prev = curr;
                }
                row--;
            }

            // Move elements of first column from remaining columns
            if (col < col.read()) {
                for (int i = row; i >= row.read(); i--) {
                    int curr = mat[i][col];
                    mat[i][col] = prev;
                    prev = curr;
                }
                col++;
            }
        }

        rot_mat.write(mat.read());
    }
};


SC_MODULE(Testbench) {
    // Using static const for matrix dimensions instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    
    // Signals to connect to the MatrixRotator ports
    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];

    MatrixRotator* matrixRotator;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixRotator module
        matrixRotator = new MatrixRotator("matrixRotator");
        // Connect the signals
        for (int i = 0; i < SIZE; i++) {
            matrixRotator->in[i](in[i]);
            matrixRotator->out[i](out[i]);
        }
        SC_THREAD(run_tests);
    }

    // Thread to initialize the matrix, check outputs, and assert correctness
    void run_tests() {
        // Initialize the matrix with test values:
        // { 1,  2,  3,  4,
        //   5,  6,  7,  8,
        //   9, 10, 11, 12,
        //  13, 14, 15, 16 }
        int init[SIZE] = {
            1, 2, 3, 4,
            5, 6, 7, 8,
            9, 10, 11, 12,
            13, 14, 15, 16
        };
        for (int i = 0; i < SIZE; i++) {
            in[i].write(init[i]);
        }

        // Wait for the signals to propagate
        wait(1, SC_NS);

        // Expected rotated matrix is:
        //  5   1   2   3
        //  9  10   6   4
        // 13  11   7   8
        // 14  15  16  12
        int expected[SIZE] = {
            5, 1, 2, 3,
            9, 10, 6, 4,
            13, 11, 7, 8,
            14, 15, 16, 12
        };

        // Check the output matrix and print the result
        std::cout << "Rotated Matrix:" << std::endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int index = i * N + j;
                int result = out[index].read();
                std::cout << result << " ";
                // Validate the result using assert
                assert(result == expected[index]);
            }
            std::cout << std::endl;
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // End the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench
    sc_start();         // Start the simulation
    return 0;
}
