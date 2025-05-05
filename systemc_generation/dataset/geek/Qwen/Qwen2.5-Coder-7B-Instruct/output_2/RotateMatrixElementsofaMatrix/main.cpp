
#include <systemc.h>

const int SIZE = 4;  // Define the size of the matrix


SC_MODULE(MatrixRotator) {
    sc_in<int> in[SIZE * SIZE];  // Input port for the matrix
    sc_out<int> out[SIZE * SIZE]; // Output port for the rotated matrix

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Process to perform matrix rotation
        SC_METHOD(rotate);
        for (int i = 0; i < SIZE * SIZE; i++) {
            sensitive << in[i];
        }
    }

    // Method to rotate the matrix
    void rotate() {
        int m = SIZE;  // Number of rows
        int n = SIZE;  // Number of columns
        int mat[SIZE][SIZE];
        int row = 0, col = 0;
        int prev, curr;

        // Copy input matrix to local matrix
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                mat[i][j] = in[i * n + j].read();
            }
        }

        while (row < m && col < n) {
            if (row + 1 == m || col + 1 == n) break;

            prev = mat[row + 1][col];

            // Rotate top row
            for (int i = col; i < n - 1; i++) {
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }

            // Rotate rightmost column
            for (int i = row; i < m - 1; i++) {
                curr = mat[i][n - 1];
                mat[i][n - 1] = prev;
                prev = curr;
            }

            // Rotate bottom row
            for (int i = n - 1; i > col; i--) {
                curr = mat[m - 1][i];
                mat[m - 1][i] = prev;
                prev = curr;
            }

            // Rotate leftmost column
            for (int i = m - 1; i > row; i--) {
                curr = mat[i][col];
                mat[i][col] = prev;
                prev = curr;
            }

            row++;
            col++;
            m -= 2;
            n -= 2;
        }

        // Copy rotated matrix to output ports
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
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
