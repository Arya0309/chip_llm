#include <systemc.h>

// Module that performs the swap of first and last columns
SC_MODULE(MatrixSwapper) {
    // Input and output ports for the flattened 4x4 matrix
    sc_in<int> in[4*4];
    sc_out<int> out[4*4];

    SC_CTOR(MatrixSwapper) {
        SC_METHOD(do_swap);
        // Make the method sensitive to all input signals
        for (int i = 0; i < 4*4; i++) {
            sensitive << in[i];
        }
    }

    // Method that swaps the first and last column of each row
    void do_swap() {
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int index = row * 4 + col;
                if (col == 0) {
                    // First column: take value from last column of input
                    out[index].write(in[row * 4 + (4 - 1)].read());
                } else if (col == 4 - 1) {
                    // Last column: take value from first column of input
                    out[index].write(in[row * 4].read());
                } else {
                    // Middle columns remain unchanged
                    out[index].write(in[index].read());
                }
            }
        }
    }
};

// Testbench module to verify the functionality of MatrixSwapper
SC_MODULE(Testbench) {

    sc_signal<int> in[4*4];
    sc_signal<int> out[4*4];

    MatrixSwapper* matrixSwapper;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixSwapper module
        matrixSwapper = new MatrixSwapper("matrixSwapper");
        // Connect the testbench signals to the module's ports
        for (int i = 0; i < 4*4; i++) {
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
        int input_matrix[4*4] = {
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
        int expected[4*4] = {
            6, 9, 7, 8,
            5, 7, 6, 4,
            8, 2, 1, 3,
            7, 9, 7, 9
        };

        // Write input matrix values into signals
        for (int i = 0; i < 4*4; i++) {
            in[i].write(input_matrix[i]);
        }

        // Allow the signals to propagate
        wait(1, SC_NS);

        // Check the swapped matrix and print the result
        std::cout << "Swapped Matrix:" << std::endl;
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int index = row * 4 + col;
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
