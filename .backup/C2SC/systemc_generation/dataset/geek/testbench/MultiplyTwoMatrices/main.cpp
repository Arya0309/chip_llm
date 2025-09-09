#include <systemc.h>

// Module that performs matrix multiplication
SC_MODULE(MatrixMultiplier) {
    // Input ports for matrices A and B (flattened as 1D arrays)
    sc_in<int> A[2*2];
    sc_in<int> B[2*3];
    // Output ports for matrix C (flattened as 1D array)
    sc_out<int> C[2*3];

    SC_CTOR(MatrixMultiplier) {
        SC_METHOD(do_mult);
        // Sensitivity to all input signals
        for (int i = 0; i < 2*2; i++) {
            sensitive << A[i];
        }
        for (int i = 0; i < 2*3; i++) {
            sensitive << B[i];
        }
    }

    // Combinational process to perform matrix multiplication
    void do_mult() {
        // For each row in Matrix A
        for (int i = 0; i < 2; i++) {
            // For each column in Matrix B
            for (int j = 0; j < 3; j++) {
                int sum = 0;
                // Compute dot product of row i of A and column j of B
                for (int k = 0; k < 2; k++) {
                    sum += A[i * 2 + k].read() * B[k * 3 + j].read();
                }
                // Write the result into the output matrix at position [i][j]
                C[i * 3 + j].write(sum);
            }
        }
    }
};

// Testbench module for MatrixMultiplier
SC_MODULE(Testbench) {
    
    // Signals to connect to the MatrixMultiplier ports
    sc_signal<int> A[2*2];
    sc_signal<int> B[2*3];
    sc_signal<int> C[2*3];

    MatrixMultiplier* mm;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixMultiplier module
        mm = new MatrixMultiplier("mm");
        // Connect signals to the module's ports
        for (int i = 0; i < 2*2; i++) {
            mm->A[i](A[i]);
        }
        for (int i = 0; i < 2*3; i++) {
            mm->B[i](B[i]);
        }
        for (int i = 0; i < 2*3; i++) {
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
        int expected[2*3] = {3, 3, 3, 6, 6, 6};

        std::cout << "Matrix multiplication result:" << std::endl;
        for (int i = 0; i < 2*3; i++) {
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
