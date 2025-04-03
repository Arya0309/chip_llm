
#include <systemc.h>

// Module to compute the transpose of a matrix
SC_MODULE(MatrixTranspose) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> rst;  // Reset signal
    sc_in<sc_matrix<int, 4, 4>> A;  // Input matrix A
    sc_out<sc_matrix<int, 4, 4>> B; // Output matrix B

    // Internal matrix to hold the result
    sc_matrix<int, 4, 4> B_internal;

    // Constructor
    SC_CTOR(MatrixTranspose) {
        // Process to perform matrix transpose
        SC_METHOD(transpose);
        sensitive << clk.pos();
        dont_initialize();
    }

    // Method to compute the transpose
    void transpose() {
        if (rst.read()) {
            // Reset the internal matrix B_internal to zero
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    B_internal[i][j] = 0;
                }
            }
        } else {
            // Compute the transpose of matrix A
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    B_internal[i][j] = A.read()[j][i];
                }
            }
        }
        // Write the result to output port B
        B.write(B_internal);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;  // Clock signal
    sc_signal<bool> rst;  // Reset signal
    sc_signal<sc_matrix<int, 4, 4>> A;  // Signal for input matrix A
    sc_signal<sc_matrix<int, 4, 4>> B;  // Signal for output matrix B

    MatrixTranspose transpose_inst; // Instance of the MatrixTranspose module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), transpose_inst("transpose_inst") {
        // Connect signals to ports
        transpose_inst.clk(clk);
        transpose_inst.rst(rst);
        transpose_inst.A(A);
        transpose_inst.B(B);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrix A
        sc_matrix<int, 4, 4> A_init = {{1, 1, 1, 1},
                                        {2, 2, 2, 2},
                                        {3, 3, 3, 3},
                                        {4, 4, 4, 4}};
        A.write(A_init);

        // Assert reset
        rst.write(true);
        wait(2, SC_NS);
        rst.write(false);

        // Wait for the transpose computation
        wait(2, SC_NS);

        // Read and print the result matrix B
        sc_matrix<int, 4, 4> B_result = B.read();
        cout << "Result matrix is \n";
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cout << " " << B_result[i][j];
            }
            cout << "\n";
        }

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
