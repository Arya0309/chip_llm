
#include <systemc.h>


SC_MODULE(MatrixRotator) {
    sc_in_clk clk;  // Clock signal for synchronization
    sc_in<int> in[4*4];  // Input ports for the matrix elements (4x4 matrix)
    sc_out<int> out[4*4]; // Output ports for the rotated matrix elements (4x4 matrix)

    int m = 4, n = 4;  // Dimensions of the matrix
    int mat[4][4];  // Matrix to store input values

    SC_CTOR(MatrixRotator) : clk("clk") {
        // Process to read input values and store them in the matrix
        SC_METHOD(read_input);
        sensitive << clk.pos();

        // Process to rotate the matrix
        SC_METHOD(rotate_matrix);
        sensitive << clk.pos();
    }

    void read_input() {
        for (int i = 0; i < m * n; i++) {
            mat[i / n][i % n] = in[i].read();
        }
    }

    void rotate_matrix() {
        int row = 0, col = 0, prev, curr;

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

            for (int i = row; i < m; i++) {
                curr = mat[i][n-1];
                mat[i][n-1] = prev;
                prev = curr;
            }
            n--;

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
        }

        // Write the rotated matrix to the output ports
        int idx = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                out[idx].write(mat[i][j]);
                idx++;
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
