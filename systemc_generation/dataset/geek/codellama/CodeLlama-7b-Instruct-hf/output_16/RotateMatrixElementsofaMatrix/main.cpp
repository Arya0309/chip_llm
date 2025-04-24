
#include <systemc.h>

// Module to rotate a matrix

SC_MODULE(MatrixRotator) {
    sc_in<int> in[4][4]; // Input port for the matrix
    sc_out<int> out[4][4]; // Output port for the rotated matrix

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Process to rotate the matrix
        SC_METHOD(rotate);
        sensitive << in;
    }

    // Method to rotate the matrix
    void rotate() {
        // Initialize the row and column indices
        int row = 0, col = 0;
        // Initialize the previous element
        int prev = in[row][col];

        // Iterate over each row and column of the matrix
        while (row < 4 && col < 4) {
            // If the next row is the last row or the next column is the last column, then break out of the loop
            if (row + 1 == 4 || col + 1 == 4) {
                break;
            }

            // Store the first element of the next row in prev
            prev = in[row + 1][col];

            // Move elements of the first row from the remaining rows to the first row by iterating over each column from the starting column to the ending column and replacing the element in the current row with prev
            for (int i = col; i < 4; i++) {
                out[row][i] = prev;
                prev = in[row][i];
            }

            // Reduce the number of rows and columns by 1
            row++;
            col++;

            // Move elements of the last column from the remaining columns to the last column by iterating over each row from the starting row to the ending row and replacing the element in the current column with prev
            for (int i = row; i < 4; i++) {
                out[i][4 - 1] = prev;
                prev = in[i][4 - 1];
            }

            // Move elements of the last row from the remaining rows to the last row by iterating over each column from the starting column to the ending column and replacing the element in the current row with prev
            for (int i = col; i < 4; i++) {
                out[4 - 1][i] = prev;
                prev = in[4 - 1][i];
            }

            // Move elements of the first column from the remaining rows to the first column by iterating over each row from the starting row to the ending row and replacing the element in the current row with prev
            for (int i = row; i < 4; i++) {
                out[i][col] = prev;
                prev = in[i][col];
            }

            // Print the rotated matrix using a cout statement
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    cout << out[i][j] << " ";
                }
                cout << endl;
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
