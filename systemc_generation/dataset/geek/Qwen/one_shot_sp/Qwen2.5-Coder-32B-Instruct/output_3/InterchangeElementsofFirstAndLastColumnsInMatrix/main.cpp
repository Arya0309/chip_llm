
#include <systemc.h>

// Use static const for constants instead of #define
static const int N = 4;
static const int SIZE = N * N;

// Module that performs row swapping

SC_MODULE(MatrixSwapper) {
    // Input ports for the matrix (flattened as 1D arrays)
    sc_in<int> in[SIZE];
    // Output ports for the matrix (flattened as 1D arrays)
    sc_out<int> out[SIZE];

    SC_CTOR(MatrixSwapper) {
        // Use an SC_METHOD to perform the row swapping operation
        SC_METHOD(swap_rows);
        for (int i = 0; i < SIZE; i++) {
            sensitive << in[i];
        }
    }

    // Method that swaps the first and last rows of the matrix
    void swap_rows() {
        int temp[N];
        // Copy the first row to a temporary array
        for (int i = 0; i < N; i++) {
            temp[i] = in[i].read();
        }
        // Write the last row to the first row
        for (int i = 0; i < N; i++) {
            out[i].write(in[(N-1)*N + i].read());
        }
        // Write the saved first row to the last row
        for (int i = 0; i < N; i++) {
            out[(N-1)*N + i].write(temp[i]);
        }
        // Copy the rest of the matrix unchanged
        for (int i = 1; i < N-1; i++) {
            for (int j = 0; j < N; j++) {
                out[i*N + j].write(in[i*N + j].read());
            }
        }
    }
};



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
