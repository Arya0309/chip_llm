
#include <systemc.h>

// Module for rotating a matrix

SC_MODULE(MatrixRotator) {
    sc_in<int> in[SIZE];  // Input port for the matrix
    sc_out<int> out[SIZE];  // Output port for the rotated matrix

    SC_CTOR(MatrixRotator) {
        SC_METHOD(rotate);
        sensitive << in;
    }

    // Method to rotate the matrix
    void rotate() {
        // Variable to store the starting row index
        int row = 0;
        // Variable to store the starting column index
        int col = 0;
        // Variable to store the previous element in the matrix
        int prev = 0;
        // Variable to store the current element in the matrix
        int curr = 0;

        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                // Store the first element of the next row
                prev = in[i + 1][j];

                // Move elements of the first row from the remaining rows
                for (int k = j; k < NUM_ROWS; k++) {
                    curr = in[i][k];
                    out[i][k] = prev;
                    prev = curr;
                }
                row++;

                // Move elements of the last column from the remaining columns
                for (int k = row; k < NUM_ROWS; k++) {
                    curr = in[k][NUM_COLS - 1];
                    out[k][NUM_COLS - 1] = prev;
                    prev = curr;
                }
                col--;

                // Move elements of the last row from the remaining rows
                if (row < NUM_ROWS) {
                    for (int k = col; k >= 0; k--) {
                        curr = in[NUM_ROWS - 1][k];
                        out[NUM_ROWS - 1][k] = prev;
                        prev = curr;
                    }
                }
                row--;

                // Move elements of the first column from the remaining columns
                if (col >= 0) {
                    for (int k = row; k >= 0; k--) {
                        curr = in[k][0];
                        out[k][0] = prev;
                        prev = curr;
                    }
                }
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
