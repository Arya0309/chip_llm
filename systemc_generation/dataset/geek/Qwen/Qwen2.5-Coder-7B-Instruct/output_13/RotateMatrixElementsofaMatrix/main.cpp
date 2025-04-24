
#include <systemc.h>


SC_MODULE(MatrixRotator) {
    sc_in<int> in[4][4];  // Input matrix of size 4x4
    sc_out<int> out[4][4]; // Output matrix of size 4x4

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Process to perform matrix rotation
        SC_THREAD(rotate_matrix);
        sensitive << in;
    }

    // Thread to perform matrix rotation
    void rotate_matrix() {
        int row = 0, col = 0;
        int prev, curr;

        while (row < 4 && col < 4) {
            if (row + 1 == 4 || col + 1 == 4)
                break;

            prev = in[row + 1][col];

            // Move elements of first row from the remaining rows
            for (int i = col; i < 4; i++) {
                curr = in[row][i];
                in[row][i] = prev;
                prev = curr;
            }
            row++;

            // Move elements of last column from the remaining columns
            for (int i = row; i < 4; i++) {
                curr = in[i][3];
                in[i][3] = prev;
                prev = curr;
            }
            3--;

            // Move elements of last row from the remaining rows
            if (row < 4) {
                for (int i = 3; i >= col; i--) {
                    curr = in[3][i];
                    in[3][i] = prev;
                    prev = curr;
                }
            }
            3--;

            // Move elements of first column from the remaining rows
            if (col < 4) {
                for (int i = 3; i >= row; i--) {
                    curr = in[i][col];
                    in[i][col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Copy the rotated matrix to the output ports
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                out[i][j].write(in[i][j]);
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
