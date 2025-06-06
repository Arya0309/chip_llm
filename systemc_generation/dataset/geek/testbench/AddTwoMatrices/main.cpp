#include <systemc.h>

// Module that performs matrix addition
SC_MODULE(MatrixAdder) {
    // Input ports for matrices A and B (flattened as 1D arrays)
    sc_in<int> A[4*4];
    sc_in<int> B[4*4];
    // Output ports for matrix C (flattened as 1D array)
    sc_out<int> C[4*4];

    SC_CTOR(MatrixAdder) {
        // Use an SC_METHOD to perform combinational matrix addition
        SC_METHOD(do_add);
        for (int i = 0; i < 4*4; i++) {
            sensitive << A[i] << B[i];
        }
    }

    // Method that adds corresponding elements of A and B
    void do_add() {
        for (int i = 0; i < 4*4; i++) {
            C[i].write(A[i].read() + B[i].read());
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {

    // Signals to connect to the MatrixAdder ports
    sc_signal<int> A[4*4];
    sc_signal<int> B[4*4];
    sc_signal<int> C[4*4];

    MatrixAdder* matrixAdder;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixAdder module
        matrixAdder = new MatrixAdder("matrixAdder");
        // Connect signals to the module's ports
        for (int i = 0; i < 4*4; i++) {
            matrixAdder->A[i](A[i]);
            matrixAdder->B[i](B[i]);
            matrixAdder->C[i](C[i]);
        }
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
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int index = row * 4 + col;
                A[index].write(row + 1); // row 0 => 1, row 1 => 2, etc.
                B[index].write(row + 1);
            }
        }

        // Wait for the signals to propagate and the combinational logic to settle.
        wait(1, SC_NS);

        // Check the result matrix C and print the output
        std::cout << "Result matrix is:" << std::endl;
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int index = row * 4 + col;
                int expected = (row + 1) + (row + 1); // Since both matrices have the same values.
                int result = C[index].read();
                // Use assert to validate the result.
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
