
#include <systemc.h>

// SystemC module for matrix multiplication

SC_MODULE(MatrixMultiplier) {
    sc_in<int> A[2]; // Input port for the first matrix (2x2)
    sc_in<int> B[3]; // Input port for the second matrix (2x3)
    sc_out<int> C[6]; // Output port for the result matrix (2x3)

    // Constructor
    SC_CTOR(MatrixMultiplier) {
        // Process to perform matrix multiplication
        SC_METHOD(multiply);
        sensitive << A[0] << A[1] << A[2] << A[3]
                  << B[0] << B[1] << B[2] << B[3] << B[4] << B[5];
    }

    // Method to multiply the two matrices
    void multiply() {
        int mat1[2][2] = {{A[0].read(), A[1].read()},
                          {A[2].read(), A[3].read()}};

        int mat2[2][3] = {{B[0].read(), B[1].read(), B[2].read()},
                          {B[3].read(), B[4].read(), B[5].read()}};

        int rslt[2][3] = {0}; // Result matrix initialized to 0

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 2; k++) {
                    rslt[i][j] += mat1[i][k] * mat2[k][j];
                }
            }
        }

        // Write the result matrix to the output ports
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                C[i*3 + j].write(rslt[i][j]);
            }
        }
    }
};


SC_MODULE(Testbench) {
    // Use static const for matrix dimensions
    static const int R1 = 2;      // Rows in Matrix-1
    static const int C1 = 2;      // Columns in Matrix-1
    static const int R2 = 2;      // Rows in Matrix-2
    static const int C2 = 3;      // Columns in Matrix-2

    static const int SIZE_A = R1 * C1;      // Total elements in Matrix-1
    static const int SIZE_B = R2 * C2;      // Total elements in Matrix-2
    static const int SIZE_RESULT = R1 * C2; // Total elements in the Result matrix
    
    // Signals to connect to the MatrixMultiplier ports
    sc_signal<int> A[SIZE_A];
    sc_signal<int> B[SIZE_B];
    sc_signal<int> C[SIZE_RESULT];

    MatrixMultiplier* mm;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixMultiplier module
        mm = new MatrixMultiplier("mm");
        // Connect signals to the module's ports
        for (int i = 0; i < SIZE_A; i++) {
            mm->A[i](A[i]);
        }
        for (int i = 0; i < SIZE_B; i++) {
            mm->B[i](B[i]);
        }
        for (int i = 0; i < SIZE_RESULT; i++) {
            mm->C[i](C[i]);
        }
        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes inputs, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize Matrix A (2x2):
        // { {1, 1},
        //   {2, 2} }
        A[0].write(1); // Row 0, Column 0
        A[1].write(1); // Row 0, Column 1
        A[2].write(2); // Row 1, Column 0
        A[3].write(2); // Row 1, Column 1

        // Initialize Matrix B (2x3):
        // { {1, 1, 1},
        //   {2, 2, 2} }
        B[0].write(1); // Row 0, Column 0
        B[1].write(1); // Row 0, Column 1
        B[2].write(1); // Row 0, Column 2
        B[3].write(2); // Row 1, Column 0
        B[4].write(2); // Row 1, Column 1
        B[5].write(2); // Row 1, Column 2

        // Wait for the signals to propagate
        wait(1, SC_NS);

        // Expected result after multiplication:
        // For result[0][0]: 1*1 + 1*2 = 3
        // For result[0][1]: 1*1 + 1*2 = 3
        // For result[0][2]: 1*1 + 1*2 = 3
        // For result[1][0]: 2*1 + 2*2 = 6
        // For result[1][1]: 2*1 + 2*2 = 6
        // For result[1][2]: 2*1 + 2*2 = 6
        int expected[SIZE_RESULT] = {3, 3, 3, 6, 6, 6};

        std::cout << "Matrix multiplication result:" << std::endl;
        for (int i = 0; i < SIZE_RESULT; i++) {
            int res = C[i].read();
            std::cout << res << " ";
            // Assert the computed result matches the expected value
            assert(res == expected[i]);
        }
        std::cout << std::endl;
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench
    sc_start();         // Start simulation
    return 0;
}
