scss
#include <systemc.h>


SC_MODULE(MatrixRotator) {
    sc_in<int> R; // Input port for the number of rows in the matrix
    sc_in<int> C; // Input port for the number of columns in the matrix
    sc_in<int> mat[R][C]; // Input port for the matrix to be rotated
    sc_out<int> rotated[R][C]; // Output port for the rotated matrix

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Process to rotate the matrix
        SC_METHOD(rotate);
        sensitive << R << C << mat;
    }

    // Method to rotate the matrix
    void rotate() {
        int row = 0, col = 0;
        int prev, curr;

        // Loop through the matrix while the row and column indices are within the bounds
        while (row < R && col < C) {
            // If the current row or column is the last row or column, break the loop
            if (row + 1 == R || col + 1 == C) {
                break;
            }

            // Store the first element of the next row
            prev = mat[row + 1][col];

            // Loop through the elements in the current row, starting from the current column
            for (int i = col; i < C; i++) {
                // Move the element to the right by one position
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }

            // Move the elements of the last column up by one position
            for (int i = row; i < R; i++) {
                // Move the element up by one position
                curr = mat[i][C - 1];
                mat[i][C - 1] = prev;
                prev = curr;
            }

            // Move the elements of the last row left by one position
            for (int i = C - 1; i >= col; i--) {
                // Move the element left by one position
                curr = mat[R - 1][i];
                mat[R - 1][i] = prev;
                prev = curr;
            }

            // Increment the row and column indices
            row++;
            col++;
        }

        // Print the rotated matrix
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                rotated[i][j].write(mat[i][j]);
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
