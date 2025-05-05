
#include <systemc.h>


SC_MODULE(MatrixRotator) {
    sc_in<int> in[4][4];  // Input matrix
    sc_out<int> out[4][4]; // Output matrix

    SC_CTOR(MatrixRotator) {
        SC_THREAD(rotatematrix);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                sensitive << in[i][j];
            }
        }
    }

    void rotatematrix() {
        int m = 4, n = 4;
        int mat[4][4];
        int row = 0, col = 0;
        int prev, curr;

        // Copy input matrix to local variable
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                mat[i][j] = in[i][j].read();
            }
        }

        while (row < m && col < n) {
            if (row + 1 == m || col + 1 == n)
                break;

            prev = mat[row + 1][col];
            for (int i = col; i < n; i++) {
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }
            row++;

            if (row < m) {
                for (int i = n-1; i >= col; i--) {
                    curr = mat[m-1][i];
                    mat[m-1][i] = prev;
                    prev = curr;
                }
            }
            m--;

            if (col < n) {
                for (int i = m-1; i >= row; i--) {
                    curr = mat[i][col];
                    mat[i][col] = prev;
                    prev = curr;
                }
            }
            col++;

            for (int i = n-1; i >= col; i--) {
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }
            row++;
        }

        // Write the rotated matrix to output
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                out[i][j].write(mat[i][j]);
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
