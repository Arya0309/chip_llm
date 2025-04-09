
#include <systemc.h>

// Use static const for constants instead of #define
static const int N = 4;
static const int SIZE = N;

// Module that performs the column interchange

SC_MODULE(MatrixSwapper) {
    // Input ports for the matrix (flattened as 1D arrays)
    sc_in<int> in[SIZE];
    // Output ports for the modified matrix (flattened as 1D array)
    sc_out<int> out[SIZE];

    SC_CTOR(MatrixSwapper) {
        // Use an SC_METHOD to perform the column interchange
        SC_METHOD(interchangeColumns);
        for (int i = 0; i < SIZE; i++) {
            sensitive << in[i];
        }
    }

    // Method that swaps the first and last columns
    void interchangeColumns() {
        for (int i = 0; i < N; i++) {
            int t = in[i * N + 0].read();  // Read first column element
            out[i * N + 0].write(in[i * N + N - 1].read());  // Write last column element to first column
            out[i * N + N - 1].write(t);  // Write saved first column element to last column
        }
        // Copy remaining elements unchanged
        for (int i = 0; i < N; i++) {
            for (int j = 1; j < N - 1; j++) {
                out[i * N + j].write(in[i * N + j].read());
            }
        }
    }
};

// Example top-level module to instantiate MatrixSwapper

SC_MODULE(TopLevel) {
    sc_signal<int> matrix_in[SIZE];
    sc_signal<int> matrix_out[SIZE];
    MatrixSwapper colInterchanger;

    SC_CTOR(TopLevel) : colInterchanger("MatrixSwapper") {
        // Connect signals
        for (int i = 0; i < SIZE; i++) {
            colInterchanger.in[i](matrix_in[i]);
            colInterchanger.out[i](matrix_out[i]);
        }
    }
};



SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;

    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];

    MatrixSwapper* matrixSwapper;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixSwapper module
        matrixSwapper = new MatrixSwapper("matrixSwapper");
        // Connect the testbench signals to the module's ports
        for (int i = 0; i < SIZE; i++) {
            matrixSwapper->in[i](in[i]);
            matrixSwapper->out[i](out[i]);
        }
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Define the input matrix as per the original C++ code:
        // { {8, 9, 7, 6},
        //   {4, 7, 6, 5},
        //   {3, 2, 1, 8},
        //   {9, 9, 7, 7} }
        int input_matrix[SIZE] = {
            8, 9, 7, 6,
            4, 7, 6, 5,
            3, 2, 1, 8,
            9, 9, 7, 7
        };

        // Expected output after swapping first and last columns:
        // { {6, 9, 7, 8},
        //   {5, 7, 6, 4},
        //   {8, 2, 1, 3},
        //   {7, 9, 7, 9} }
        int expected[SIZE] = {
            6, 9, 7, 8,
            5, 7, 6, 4,
            8, 2, 1, 3,
            7, 9, 7, 9
        };

        // Write input matrix values into signals
        for (int i = 0; i < SIZE; i++) {
            in[i].write(input_matrix[i]);
        }

        // Allow the signals to propagate
        wait(1, SC_NS);

        // Check the swapped matrix and print the result
        std::cout << "Swapped Matrix:" << std::endl;
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                int result = out[index].read();
                // Assert that the result matches the expected value
                assert(result == expected[index]);
                std::cout << result << " ";
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
