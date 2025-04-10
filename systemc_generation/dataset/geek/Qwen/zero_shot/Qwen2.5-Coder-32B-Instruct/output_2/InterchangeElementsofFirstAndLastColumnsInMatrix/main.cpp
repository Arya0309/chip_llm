
#include <systemc.h>

#define SIZE 4


SC_MODULE(MatrixSwapper) {
    sc_in<int> in[SIZE*SIZE];
    sc_out<int> out[SIZE*SIZE];

    SC_CTOR(MatrixSwapper) {
        SC_METHOD(swapRows);
        sensitive << in;
    }

    void swapRows() {
        int m[SIZE][SIZE];
        int swappedM[SIZE][SIZE];

        // Copy input to local matrix
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                m[i][j] = in[i*SIZE + j].read();
            }
        }

        // Swap the first and last rows
        for (int i = 0; i < SIZE; ++i) {
            int temp = m[0][i];
            m[0][i] = m[SIZE - 1][i];
            m[SIZE - 1][i] = temp;
        }

        // Copy the swapped matrix to output
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                out[i*SIZE + j].write(m[i][j]);
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
