
#include <systemc.h>

// Use static const for constants instead of #define
static const int R = 4;
static const int C = 4;
static const int SIZE = R * C;

// Module that performs matrix rotation

SC_MODULE(MatrixRotator) {
    // Input ports for the matrix (flattened as 1D arrays)
    sc_in<int> in[SIZE];
    // Output ports for the rotated matrix (flattened as 1D array)
    sc_out<int> out[SIZE];

    // Internal storage for the matrix
    int mat[R][C];

    SC_CTOR(MatrixRotator) {
        // Use an SC_METHOD to perform the matrix rotation
        SC_METHOD(rotatematrix);
        sensitive << in[0]; // Sensitive to changes in the first element
    }

    // Method that rotates the matrix
    void rotatematrix() {
        // Copy input matrix to internal storage
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                mat[i][j] = in[i * C + j].read();
            }
        }

        int row = 0, col = 0;
        int prev, curr;

        while (row < R && col < C) {
            if (row + 1 == R || col + 1 == C) break;

            // Store the first element of the next row
            prev = mat[row + 1][col];

            // Move elements of the first row
            for (int i = col; i < C; ++i) {
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }
            row++;

            // Move elements of the last column
            for (int i = row; i < R; ++i) {
                curr = mat[i][C - 1];
                mat[i][C - 1] = prev;
                prev = curr;
            }
            C--;

            // Move elements of the last row
            if (row < R) {
                for (int i = C - 1; i >= col; --i) {
                    curr = mat[R - 1][i];
                    mat[R - 1][i] = prev;
                    prev = curr;
                }
            }
            R--;

            // Move elements of the first column
            if (col < C) {
                for (int i = R - 1; i >= row; --i) {
                    curr = mat[i][col];
                    mat[i][col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Write the rotated matrix to the output ports
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                out[i * C + j].write(mat[i][j]);
            }
        }
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
