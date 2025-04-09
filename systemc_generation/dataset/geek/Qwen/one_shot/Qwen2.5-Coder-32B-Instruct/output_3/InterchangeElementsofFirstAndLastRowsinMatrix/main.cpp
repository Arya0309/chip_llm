
#include <systemc.h>

// Module to interchange first and last columns of a 4x4 matrix

SC_MODULE(MatrixSwapper) {
    sc_in<int> in[4*4];  // Input ports for the 4x4 matrix elements
    sc_out<int> out[4*4]; // Output ports for the 4x4 matrix elements

    // Constructor
    SC_CTOR(MatrixSwapper) {
        // Process to perform column interchange
        SC_METHOD(interchangeColumns);
        sensitive << in[0]; // Sensitivity list can be expanded if needed
    }

    // Method to interchange first and last columns
    void interchangeColumns() {
        int m[4][4];
        int m_out[4][4];

        // Reading input matrix from input ports
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = in[i*4 + j].read();
            }
        }

        // Swapping the first and last columns
        for (int i = 0; i < 4; i++) {
            int t = m[i][0];
            m[i][0] = m[i][3];
            m[i][3] = t;
        }

        // Writing the modified matrix to output ports
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m_out[i][j] = m[i][j];
                out[i*4 + j].write(m_out[i][j]);
            }
        }
    }
};

// Example usage in a testbench


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
