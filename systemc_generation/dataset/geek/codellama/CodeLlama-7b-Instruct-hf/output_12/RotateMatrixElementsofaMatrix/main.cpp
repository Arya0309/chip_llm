
#include <systemc.h>

// Module that rotates a matrix

SC_MODULE(MatrixRotator) {
    sc_out<int> out[SIZE]; // Output port for the rotated matrix
    sc_in<int> in[SIZE]; // Input port for the matrix to be rotated

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Process to perform the rotation
        SC_METHOD(rotate);
        sensitive << in[0] << in[1] << in[2] << in[3];
    }

    // Method to rotate the matrix
    void rotate() {
        // Code for the first block of the original code
        for (int i = 0; i < R && i < C; i++) {
            out[i][n-1] = in[i][C - 1];
        }
        for (int i = 0; i < R && i < C; i++) {
            for (int j = n-1; j > i; j--) {
                in[i][j] = out[i][j-1];
            }
        }
        n--;
        // Code for the remaining blocks of the original code
        for (int i = 0; i < R && i < C; i += 2) {
            out[i][n-1] = in[i][C - 1];
        }
        for (int i = 1; i < R && i < C; i += 2) {
            out[i][n-1] = in[i][C - 1];
        }
        for (int i = 0; i < R && i < C; i++) {
            for (int j = n-1; j > i; j--) {
                in[i][j] = out[i][j-1];
            }
        }
        for (int i = 1; i < R && i < C; i++) {
            for (int j = n-1; j > i; j--) {
                in[i][j] = out[i][j-1];
            }
        }
        out[0][0] = in[R-1][C-1];
        out[1][0] = in[R-2][C-1];
        out[2][0] = in[R-3][C-1];
        out[3][0] = in[R-4][C-1];
        out[0][1] = in[R-1][C-2];
        out[1][1] = in[R-2][C-2];
        out[2][1] = in[R-3][C-2];
        out[3][1] = in[R-4][C-2];
        ...
        // Print rotated matrix
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                cout << out[i][j] << " ";
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
