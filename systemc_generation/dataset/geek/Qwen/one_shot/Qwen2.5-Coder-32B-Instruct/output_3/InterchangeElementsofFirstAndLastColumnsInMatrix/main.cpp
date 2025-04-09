
#include <systemc.h>

// Module to interchange the first and last row of a 4x4 matrix

SC_MODULE(MatrixSwapper) {
    sc_in<int> in[4*4];  // Input ports for the 4x4 matrix elements
    sc_out<int> out[4*4]; // Output ports for the 4x4 matrix elements

    // Constructor
    SC_CTOR(MatrixSwapper) {
        // Process to perform row interchange
        SC_METHOD(interchangeRows);
        sensitive << in[0] << in[1] << in[2] << in[3] << in[4] << in[5] << in[6] << in[7] << in[8] << in[9] << in[10] << in[11] << in[12] << in[13] << in[14] << in[15];
    }

    // Method to interchange the first and last rows
    void interchangeRows() {
        int matrix[4][4];

        // Read the input matrix
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                matrix[i][j] = in[i*4 + j].read();
            }
        }

        // Swap the first and last rows
        for (int i = 0; i < 4; ++i) {
            int temp = matrix[0][i];
            matrix[0][i] = matrix[3][i];
            matrix[3][i] = temp;
        }

        // Write the output matrix
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out[i*4 + j].write(matrix[i][j]);
            }
        }
    }
};

// Example usage


SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    
    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];
    MatrixSwapper* swapper;

    SC_CTOR(Testbench) {
        swapper = new MatrixSwapper("swapper");
        // Connect the signals to the MatrixSwapper ports
        for (int i = 0; i < SIZE; i++) {
            swapper->in[i](in[i]);
            swapper->out[i](out[i]);
        }
        SC_THREAD(run_tests);
    }

    // Thread that runs the test, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize the matrix as in the original C++ code:
        //   { {8, 9, 7, 6},
        //     {4, 7, 6, 5},
        //     {3, 2, 1, 8},
        //     {9, 9, 7, 7} }
        int m[SIZE] = {8, 9, 7, 6,
                       4, 7, 6, 5,
                       3, 2, 1, 8,
                       9, 9, 7, 7};
        for (int i = 0; i < SIZE; i++) {
            in[i].write(m[i]);
        }

        // Wait for signals to propagate
        wait(1, SC_NS);

        // Expected output after swapping first and last rows:
        //   { {9, 9, 7, 7},
        //     {4, 7, 6, 5},
        //     {3, 2, 1, 8},
        //     {8, 9, 7, 6} }
        int expected[SIZE] = {9, 9, 7, 7,
                              4, 7, 6, 5,
                              3, 2, 1, 8,
                              8, 9, 7, 6};

        // Print and check the result matrix
        std::cout << "Swapped Matrix:" << std::endl;
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int idx = row * N + col;
                int result = out[idx].read();
                std::cout << result << " ";
                assert(result == expected[idx]);
            }
            std::cout << std::endl;
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench
    sc_start();         // Start the simulation
    return 0;
}
