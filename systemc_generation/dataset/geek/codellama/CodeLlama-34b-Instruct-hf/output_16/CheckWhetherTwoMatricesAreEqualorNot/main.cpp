
#include <systemc.h>

#define N 4

// Module that compares two matrices

SC_MODULE(MatrixComparator) {
    sc_in<bool> identical; // Input port for the comparison result
    sc_in<int> A[N][N];    // Input port for the first matrix
    sc_in<int> B[N][N];    // Input port for the second matrix

    // Constructor
    SC_CTOR(MatrixComparator) {
        // Process to perform comparison
        SC_METHOD(compare);
        sensitive << A << B;
    }

    // Method to compare the matrices
    void compare() {
        // Check if the sizes of the matrices are the same
        bool sameSize = (A.getSize() == B.getSize());

        // Iterate through the elements of the matrices
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                // Check if the corresponding elements are the same
                bool sameElement = (A[i][j] == B[i][j]);

                // If any element is different, set the comparison result to false
                if (!sameElement) {
                    identical.write(false);
                    return;
                }
            }
        }

        // If all elements are the same, set the comparison result to true
        identical.write(true);
    }
};


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
