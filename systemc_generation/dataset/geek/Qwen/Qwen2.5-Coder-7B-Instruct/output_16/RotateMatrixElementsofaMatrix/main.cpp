
#include <systemc.h>


SC_MODULE(MatrixRotator) {
    static const int SIZE = 4; // Assuming a 4x4 matrix for simplicity
    sc_in<int> in[SIZE * SIZE]; // Input matrix elements
    sc_out<int> out[SIZE * SIZE]; // Output matrix elements

    SC_CTOR(MatrixRotator) {
        SC_THREAD(rotate_matrix);
    }

    void rotate_matrix() {
        int mat[SIZE][SIZE];

        // Copy input matrix elements to local array
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                mat[i][j] = in[i * SIZE + j].read();
            }
        }

        // Rotate the matrix
        int row = 0, col = 0;
        int prev, curr;
        while (row < SIZE && col < SIZE) {
            if (row + 1 == SIZE || col + 1 == SIZE) break;

            prev = mat[row + 1][col];
            for (int i = col; i < SIZE; ++i) {
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }
            row++;

            for (int i = row; i < SIZE; ++i) {
                curr = mat[i][SIZE - 1];
                mat[i][SIZE - 1] = prev;
                prev = curr;
            }
            SIZE--;

            if (row < SIZE) {
                for (int i = SIZE - 1; i >= col; --i) {
                    curr = mat[SIZE - 1][i];
                    mat[SIZE - 1][i] = prev;
                    prev = curr;
                }
            }
            SIZE--;

            if (col < SIZE) {
                for (int i = SIZE - 1; i >= row; --i) {
                    curr = mat[i][col];
                    mat[i][col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Copy rotated matrix elements to output array
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                out[i * SIZE + j].write(mat[i][j]);
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
