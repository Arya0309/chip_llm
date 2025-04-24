SystemC
// File: MatrixTranspose.sc

#include <systemc.h>

// Struct to represent the input and output of the module
struct MatrixTranspose_module {
    sc_in<int> A[N][N]; // Input matrix
    sc_out<int> B[N][N]; // Output matrix
};

// Module definition

SC_MODULE(MatrixTranspose) {
    void MatrixTranspose(int A[N][N], int B[N][N]);
    void clk(); // Clock process

    // Constructor
    SC_CTOR(MatrixTranspose) {
        SC_CTHREAD(tmp, clk.pos());
    }
};

// Thread process
void MatrixTranspose::clk()
{
    int A[N][N], i, j, B[N][N];
    // Initialize A and B to the correct values
    A[0][0] = 1; A[0][1] = 2; A[0][2] = 3; A[0][3] = 4;
    A[1][0] = 1; A[1][1] = 2; A[1][2] = 3; A[1][3] = 4;
    A[2][0] = 1; A[2][1] = 2; A[2][2] = 3; A[2][3] = 4;
    A[3][0] = 1; A[3][1] = 2; A[3][2] = 3; A[3][3] = 4;
    B[0][0] = 1; B[0][1] = 2; B[0][2] = 3; B[0][3] = 4;
    B[1][0] = 1; B[1][1] = 2; B[1][2] = 3; B[1][3] = 4;
    B[2][0] = 1; B[2][1] = 2; B[2][2] = 3; B[2][3] = 4;
    B[3][0] = 1; B[3][1] = 2; B[3][2] = 3; B[3][3] = 4;
    while (1) {
        // Transpose A and store in B
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                B[i][j] = A[j][i];
            }
        }
        wait();
    }
}

// Module created based on the testbench given
#include "MatrixTranspose.sc"



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
