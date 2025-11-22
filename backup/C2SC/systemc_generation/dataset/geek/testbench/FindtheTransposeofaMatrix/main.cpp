#include <systemc.h>

// Module that performs the matrix transpose operation
SC_MODULE(MatrixTranspose) {
    // Input ports: flattened 1D array representation of the matrix A
    sc_in<int> A[4*4];
    // Output ports: flattened 1D array representation of the transposed matrix B
    sc_out<int> B[4*4];

    SC_CTOR(MatrixTranspose) {
        // Use an SC_METHOD to perform combinational logic
        SC_METHOD(do_transpose);
        // Sensitivity list: any change in any element of A triggers the method
        for (int i = 0; i < 4*4; i++) {
            sensitive << A[i];
        }
    }

    // Method that transposes matrix A into matrix B
    void do_transpose() {
        // Loop over each element of the 4x4 matrix
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                // Use flattened indexing:
                // B[i*N + j] = A[j*N + i] implements the transpose: B[i][j] = A[j][i]
                B[i * 4 + j].write(A[j * 4 + i].read());
            }
        }
    }
};

// Testbench module to drive and verify the MatrixTranspose module
SC_MODULE(Testbench) {
    
    // Signals to connect to the MatrixTranspose ports
    sc_signal<int> A[4*4];
    sc_signal<int> B[4*4];

    MatrixTranspose* transpose_inst;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixTranspose module
        transpose_inst = new MatrixTranspose("MatrixTranspose");
        // Connect the signals to the module's ports
        for (int i = 0; i < 4*4; i++) {
            transpose_inst->A[i](A[i]);
            transpose_inst->B[i](B[i]);
        }
        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes inputs, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize matrix A as defined in the original C++ code:
        // A = { {1, 1, 1, 1},
        //       {2, 2, 2, 2},
        //       {3, 3, 3, 3},
        //       {4, 4, 4, 4} }
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int index = row * 4 + col;
                A[index].write(row + 1); // row 0 -> 1, row 1 -> 2, etc.
            }
        }

        // Wait for the signals to propagate and the combinational logic to settle.
        wait(1, SC_NS);

        // Check the result transposed matrix B and print the output
        std::cout << "Result matrix is:" << std::endl;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int index = i * 4 + j;
                // Expected value: since B[i][j] = A[j][i] and each row j of A is (j+1)
                int expected = j + 1;
                int result = B[index].read();
                // Validate the result using assert
                assert(result == expected);
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
