
#include <systemc.h>

// Define constants using static const for better readability and scalability
static const int N = 4;
static const int SIZE = N * N;

// Module to check if two matrices are identical

SC_MODULE(MatrixComparator) {
    // Input ports for matrices A and B (flattened as 1D arrays)
    sc_in<int> A[SIZE];
    sc_in<int> B[SIZE];
    // Output port for the result (whether the matrices are identical)
    sc_out<bool> identical;

    SC_CTOR(MatrixComparator) {
        // Use an SC_METHOD to perform combinational comparison
        SC_METHOD(compare_matrices);
        for (int i = 0; i < SIZE; i++) {
            sensitive << A[i] << B[i];
        }
    }

    // Method that compares corresponding elements of A and B
    void compare_matrices() {
        bool are_identical = true;
        for (int i = 0; i < SIZE; i++) {
            if (A[i].read() != B[i].read()) {
                are_identical = false;
                break;
            }
        }
        identical.write(are_identical);
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
