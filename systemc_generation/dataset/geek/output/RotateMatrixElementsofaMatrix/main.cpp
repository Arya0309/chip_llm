
#include <systemc.h>

// Constants for matrix size
static const int N = 4;
static const int SIZE = N * N;

// Module that performs matrix rotation

SC_MODULE(MatrixRotator) {
    // Input ports for the matrix (flattened as 1D arrays)
    sc_in<int> in[SIZE];
    // Output ports for the rotated matrix (flattened as 1D array)
    sc_out<int> out[SIZE];

    SC_CTOR(MatrixRotator) {
        // Use an SC_METHOD to perform the rotation
        SC_METHOD(rotate_matrix);
        sensitive << in; // Sensitive to changes in input matrix
    }

    // Method that rotates the matrix 90 degrees clockwise
    void rotate_matrix() {
        int mat[N][N];
        int rotated_mat[N][N];

        // Copy input matrix to local array
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                mat[i][j] = in[i*N + j].read();
            }
        }

        // Perform the rotation
        int row = 0, col = 0;
        int prev, curr;
        while (row < N && col < N) {
            if (row + 1 == N || col + 1 == N)
                break;

            prev = mat[row + 1][col];
            for (int i = col; i < N; i++) {
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }
            row++;

            for (int i = row; i < N; i++) {
                curr = mat[i][N-1];
                mat[i][N-1] = prev;
                prev = curr;
            }
            N--;

            if (row < N) {
                for (int i = N-1; i >= col; i--) {
                    curr = mat[N-1][i];
                    mat[N-1][i] = prev;
                    prev = curr;
                }
            }
            N--;

            if (col < N) {
                for (int i = N-1; i >= row; i--) {
                    curr = mat[i][col];
                    mat[i][col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Copy rotated matrix to output ports
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                out[i*N + j].write(mat[i][j]);
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
