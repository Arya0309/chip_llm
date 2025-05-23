#include <systemc.h>

// Use static const for constants instead of #define
static const int N = 4;
static const int SIZE = N * N;

// Module that swaps the first and last rows of a 4x4 matrix
SC_MODULE(MatrixSwapper) {
    // Input ports for the matrix elements (flattened into a 1D array)
    sc_in<int> in[SIZE];
    // Output ports for the swapped matrix elements (flattened into a 1D array)
    sc_out<int> out[SIZE];

    SC_CTOR(MatrixSwapper) {
        SC_METHOD(do_swap);
        for (int i = 0; i < SIZE; i++) {
            sensitive << in[i];
        }
    }

    // Method to perform the swapping of first and last rows
    void do_swap() {
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int idx = row * N + col;
                if (row == 0) {
                    // For the first row, output the element from the last row
                    int swap_index = (N - 1) * N + col;
                    out[idx].write(in[swap_index].read());
                } else if (row == N - 1) {
                    // For the last row, output the element from the first row
                    int swap_index = col;
                    out[idx].write(in[swap_index].read());
                } else {
                    // Other rows remain unchanged
                    out[idx].write(in[idx].read());
                }
            }
        }
    }
};

// Testbench module for the MatrixSwapper
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
