
#include <systemc.h>

// Module that computes the transpose of a matrix

SC_MODULE(MatrixTranspose) {
    sc_in<int> A[SIZE];  // Input port for the matrix A
    sc_out<int> B[SIZE]; // Output port for the transposed matrix B

    // Constructor
    SC_CTOR(MatrixTranspose) {
        // Process to compute the transpose
        SC_METHOD(computeTranspose);
        sensitive << A[0] << A[1] << A[2] << A[3] << A[4] << A[5] << A[6] << A[7] << A[8] << A[9] << A[10] << A[11] << A[12] << A[13] << A[14] << A[15];
    }

    // Method to compute the transpose of the matrix
    void computeTranspose() {
        const int N = 4;
        int tempA[N][N];
        int tempB[N][N];

        // Load the input matrix into a temporary array
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                tempA[i][j] = A[i * N + j].read();
            }
        }

        // Compute the transpose
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                tempB[i][j] = tempA[j][i];
            }
        }

        // Write the transposed matrix to the output
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i * N + j].write(tempB[i][j]);
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
