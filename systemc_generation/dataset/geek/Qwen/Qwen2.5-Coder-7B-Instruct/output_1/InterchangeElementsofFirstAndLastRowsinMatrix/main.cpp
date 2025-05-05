
#include <systemc.h>

#define SIZE 4  // Define the size of the matrix

// Module that swaps the first and last elements of each row in a matrix

SC_MODULE(MatrixSwapper) {
    sc_in<int> in[SIZE];  // Input ports for the matrix elements
    sc_out<int> out[SIZE]; // Output ports for the swapped matrix elements

    // Constructor
    SC_CTOR(MatrixSwapper) {
        // Process to swap the first and last elements of each row
        SC_METHOD(interchangeFirstLast);
        sensitive << in;
    }

    // Method to swap the first and last elements of each row
    void interchangeFirstLast() {
        for (int i = 0; i < SIZE; i++) {
            int t = in[i].read();
            out[i].write(t);
        }
    }
};

// Testbench module to drive the MatrixSwapper module


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
