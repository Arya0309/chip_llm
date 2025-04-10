
#include <systemc.h>


SC_MODULE(MatrixComparator) {
    // Define the provided signals
    sc_in<int> A[4][4];
    sc_in<int> B[4][4];
    sc_out<bool> identical;

    SC_CTOR(MatrixComparator) {
        SC_METHOD(compare_matrices);
        sensitive << A[0][0] << A[0][1] << A[0][2] << A[0][3]
                  << A[1][0] << A[1][1] << A[1][2] << A[1][3]
                  << A[2][0] << A[2][1] << A[2][2] << A[2][3]
                  << A[3][0] << A[3][1] << A[3][2] << A[3][3]
                  << B[0][0] << B[0][1] << B[0][2] << B[0][3]
                  << B[1][0] << B[1][1] << B[1][2] << B[1][3]
                  << B[2][0] << B[2][1] << B[2][2] << B[2][3]
                  << B[3][0] << B[3][1] << B[3][2] << B[3][3];
    }

    void compare_matrices() {
        bool matrices_are_identical = true;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (A[i][j].read() != B[i][j].read()) {
                    matrices_are_identical = false;
                    break;
                }
            }
            if (!matrices_are_identical) break;
        }
        identical.write(matrices_are_identical);
    }
};

// Example testbench (not included in the final module)
// 

SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    sc_signal<int> A[SIZE];
    sc_signal<int> B[SIZE];
    sc_signal<bool> result;

    MatrixComparator* comparator;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixComparator module
        comparator = new MatrixComparator("comparator");
        // Connect signals to the module's ports
        for (int i = 0; i < SIZE; i++) {
            comparator->A[i](A[i]);
            comparator->B[i](B[i]);
        }
        comparator->identical(result);

        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes inputs, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize matrices A and B as defined in the original C++ code:
        // Matrix A:
        //   { 1, 1, 1, 1 },
        //   { 2, 2, 2, 2 },
        //   { 3, 3, 3, 3 },
        //   { 4, 4, 4, 4 }
        // Matrix B: identical to Matrix A.
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                A[index].write(row + 1); // For row 0, write 1; row 1, write 2; etc.
                B[index].write(row + 1);
            }
        }

        // Wait for the signals to propagate and for the comparison to be performed.
        wait(1, SC_NS);

        // Check the result and print the output
        if (result.read() == true) {
            std::cout << "Matrices are identical" << std::endl;
        } else {
            std::cout << "Matrices are not identical" << std::endl;
        }

        // Assert the result is true, as the matrices are identical.
        assert(result.read() == true);
        std::cout << "Test passed successfully" << std::endl;

        sc_stop(); // End the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench
    sc_start();         // Start the simulation
    return 0;
}
