#include <systemc.h>

// Module that swaps the first and last rows of a 4x4 matrix
SC_MODULE(MatrixSwapper) {
    // Input ports for the matrix elements (flattened into a 1D array)
    sc_in<int> in[4*4];
    // Output ports for the swapped matrix elements (flattened into a 1D array)
    sc_out<int> out[4*4];

    SC_CTOR(MatrixSwapper) {
        SC_METHOD(do_swap);
        for (int i = 0; i < 4*4; i++) {
            sensitive << in[i];
        }
    }

    // Method to perform the swapping of first and last rows
    void do_swap() {
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int idx = row * 4 + col;
                if (row == 0) {
                    // For the first row, output the element from the last row
                    int swap_index = (4 - 1) * 4 + col;
                    out[idx].write(in[swap_index].read());
                } else if (row == 4 - 1) {
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

    
    sc_signal<int> in[4*4];
    sc_signal<int> out[4*4];
    MatrixSwapper* swapper;

    SC_CTOR(Testbench) {
        swapper = new MatrixSwapper("swapper");
        // Connect the signals to the MatrixSwapper ports
        for (int i = 0; i < 4*4; i++) {
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
        int m[4*4] = {8, 9, 7, 6,
                       4, 7, 6, 5,
                       3, 2, 1, 8,
                       9, 9, 7, 7};
        for (int i = 0; i < 4*4; i++) {
            in[i].write(m[i]);
        }

        // Wait for signals to propagate
        wait(1, SC_NS);

        // Expected output after swapping first and last rows:
        //   { {9, 9, 7, 7},
        //     {4, 7, 6, 5},
        //     {3, 2, 1, 8},
        //     {8, 9, 7, 6} }
        int expected[4*4] = {9, 9, 7, 7,
                              4, 7, 6, 5,
                              3, 2, 1, 8,
                              8, 9, 7, 6};

        // Print and check the result matrix
        std::cout << "Swapped Matrix:" << std::endl;
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int idx = row * 4 + col;
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
