
#include <systemc.h>


SC_MODULE(MatrixTranspose) {
    sc_in<int> A[16]; // Input matrix A (flattened)
    sc_out<int> B[16]; // Output matrix B (flattened)

    SC_CTOR(MatrixTranspose) {
        SC_METHOD(transpose);
        sensitive << A[0] << A[1] << A[2] << A[3] << A[4] << A[5] << A[6] << A[7] << A[8] << A[9] << A[10] << A[11] << A[12] << A[13] << A[14] << A[15];
    }

    void transpose() {
        int B_local[4][4]; // Local copy of B for computation
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                B_local[i][j] = A[j * 4 + i].read(); // Read from input and transpose
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                B[i * 4 + j].write(B_local[i][j]); // Write to output
            }
        }
    }
};


SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    
    // Signals to connect to the MatrixTranspose ports
    sc_signal<int> A[SIZE];
    sc_signal<int> B[SIZE];

    MatrixTranspose* transpose_inst;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixTranspose module
        transpose_inst = new MatrixTranspose("MatrixTranspose");
        // Connect the signals to the module's ports
        for (int i = 0; i < SIZE; i++) {
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
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                A[index].write(row + 1); // row 0 -> 1, row 1 -> 2, etc.
            }
        }

        // Wait for the signals to propagate and the combinational logic to settle.
        wait(1, SC_NS);

        // Check the result transposed matrix B and print the output
        std::cout << "Result matrix is:" << std::endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int index = i * N + j;
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
