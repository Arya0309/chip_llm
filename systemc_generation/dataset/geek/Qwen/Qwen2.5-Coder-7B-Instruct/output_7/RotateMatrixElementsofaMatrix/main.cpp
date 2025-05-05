
#include <systemc.h>

const int R = 4;
const int C = 4;


SC_MODULE(MatrixRotator) {
    sc_in<int> in[R * C];
    sc_out<int> out[R * C];

    int mat[R][C];
    int m, n;

    SC_CTOR(MatrixRotator) : m(R), n(C) {
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                mat[i][j] = 0; // Initialize to zero or appropriate default value
            }
        }
        SC_THREAD(rotatematrix);
        for (int i = 0; i < R * C; ++i) {
            sensitive << in[i];
        }
    }

    void rotatematrix() {
        int row = 0, col = 0;
        int prev, curr;
        int total = m * n;

        while (total > 0) {
            int count = 0;

            // Rotate elements of the first row
            for (int i = col; i < n - 1; i++) {
                prev = mat[row][i];
                mat[row][i] = mat[row][i + 1];
                count++;
            }

            // Rotate elements of the last column
            for (int i = row; i < m - 1; i++) {
                curr = mat[i][n - 1];
                mat[i][n - 1] = prev;
                prev = curr;
                count++;
            }

            // Rotate elements of the last row
            for (int i = n - 1; i > col; i--) {
                curr = mat[m - 1][i];
                mat[m - 1][i] = prev;
                prev = curr;
                count++;
            }

            // Rotate elements of the first column
            for (int i = m - 1; i > row; i--) {
                curr = mat[i][col];
                mat[i][col] = prev;
                prev = curr;
                count++;
            }

            // Update the total number of elements to be rotated
            total -= count;

            // Move to the next layer of the matrix
            row++;
            col++;
            m--;
            n--;
        }

        // Output the rotated matrix
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
