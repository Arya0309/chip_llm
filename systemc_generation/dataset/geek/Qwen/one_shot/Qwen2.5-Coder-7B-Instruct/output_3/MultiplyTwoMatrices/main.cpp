
#include <systemc.h>


SC_MODULE(MatrixMultiplier) {
    sc_in<int> A[SIZE_A];  // Input port for Matrix-1
    sc_in<int> B[SIZE_B];  // Input port for Matrix-2
    sc_out<int> C[SIZE_RESULT]; // Output port for the result matrix

    // Constructor
    SC_CTOR(MatrixMultiplier) {
        // Process to perform matrix multiplication
        SC_THREAD(mulMat);
        sensitive << A << B;
    }

    // Method to multiply the matrices
    void mulMat() {
        int rslt[SIZE_A][SIZE_B];

        for (int i = 0; i < SIZE_A; i++) {
            for (int j = 0; j < SIZE_B; j++) {
                rslt[i][j] = 0;
                for (int k = 0; k < SIZE_A; k++) {
                    rslt[i][j] += A[i*SIZE_A + k] * B[k*SIZE_B + j];
                }
                C[i*SIZE_B + j].write(rslt[i][j]);
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
