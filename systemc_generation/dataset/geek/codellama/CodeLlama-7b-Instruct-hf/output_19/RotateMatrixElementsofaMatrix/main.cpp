c
 #include <systemc.h>

// Module to rotate a matrix

SC_MODULE(MatrixRotator) {

    // Ports
    sc_in<int> in[SIZE]; // Input port for the matrix
    sc_out<int> out[SIZE]; // Output port for the rotated matrix

    // Constructor
    SC_CTOR(MatrixRotator) {

        // 2D array to store the matrix and the
        // rotated matrix
        int mat[R][C];
        int rot_mat[R][C];

        // Loop to iterate over each element of the array
        // and apply the rotation logic
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {

                // Calculate the index for the current element
                int index = i * C + j;

                // Move the first element of each row to the last column of the same row
                if (i == 0 && j < C - 1) {
                    rot_mat[i][j] = mat[i][j + 1];
                }

                // Move the first element of each column to the last row of the same column
                if (j == 0 && i < R - 1) {
                    rot_mat[i][j] = mat[i + 1][j];
                }

                // Move the last element of each row to the first column of the same row
                if (i == R - 1 && j > 0) {
                    rot_mat[i][j] = mat[i][j - 1];
                }

                // Move the last element of each column to the first row of the same column
                if (j == C - 1 && i > 0) {
                    rot_mat[i][j] = mat[i - 1][j];
                }
            }
        }

        // Print the rotated matrix
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                cout << rot_mat[i][j] << " ";
            }
            cout << endl;
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
