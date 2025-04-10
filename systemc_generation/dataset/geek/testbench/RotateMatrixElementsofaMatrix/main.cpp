#include <systemc.h>

// Using static const for matrix dimensions instead of #define
static const int N = 4;
static const int SIZE = N * N;

// Module that performs matrix rotation
SC_MODULE(MatrixRotator) {
    // Input ports for the matrix (flattened as a 1D array)
    sc_in<int> in[SIZE];
    // Output ports for the rotated matrix (flattened as a 1D array)
    sc_out<int> out[SIZE];

    SC_CTOR(MatrixRotator) {
        SC_METHOD(do_rotate);
        for (int i = 0; i < SIZE; i++) {
            sensitive << in[i];
        }
    }

    // Method that rotates the matrix by one anti-clockwise ring
    void do_rotate() {
        int mat[N][N];
        // Read input signals into a 2D local matrix
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                mat[i][j] = in[i * N + j].read();
            }
        }

        int row = 0, col = 0;
        int m = N, n = N;
        int prev, curr;

        // Process one ring (layer) at a time
        while (row < m && col < n) {
            if (row + 1 == m || col + 1 == n)
                break;

            // Store the first element of the next row
            prev = mat[row + 1][col];

            // Move elements of the first row from left to right
            for (int i = col; i < n; i++) {
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }
            row++;

            // Move elements of the last column from top to bottom
            for (int i = row; i < m; i++) {
                curr = mat[i][n - 1];
                mat[i][n - 1] = prev;
                prev = curr;
            }
            n--;

            // Move elements of the last row from right to left
            if (row < m) {
                for (int i = n - 1; i >= col; i--) {
                    curr = mat[m - 1][i];
                    mat[m - 1][i] = prev;
                    prev = curr;
                }
            }
            m--;

            // Move elements of the first column from bottom to top
            if (col < n) {
                for (int i = m - 1; i >= row; i--) {
                    curr = mat[i][col];
                    mat[i][col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Write the rotated matrix back to the output ports
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                out[i * N + j].write(mat[i][j]);
            }
        }
    }
};

// Testbench module
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
