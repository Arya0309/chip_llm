
#include <systemc.h>

const int N = 4;

// Module to compare two matrices
SC_MODULE(MatrixComparator) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> rst;          // Reset signal
    sc_in<int> A[N][N];      // Input matrix A
    sc_in<int> B[N][N];      // Input matrix B
    sc_out<bool> identical;   // Output indicating if matrices are identical

    bool is_identical;

    // Constructor
    SC_CTOR(MatrixComparator) {
        // Register the comparison process
        SC_METHOD(compare_matrices);
        sensitive << clk.posedge_event() << rst.posedge_event();
    }

    // Method to compare matrices
    void compare_matrices() {
        if (rst.read()) {
            is_identical = true;
            identical.write(false);
        } else {
            is_identical = true;
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    if (A[i][j].read() != B[i][j].read()) {
                        is_identical = false;
                        break;
                    }
                }
                if (!is_identical) break;
            }
            identical.write(is_identical);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;             // Clock signal
    sc_signal<bool> rst;      // Reset signal
    sc_signal<int> A[N][N];   // Signals for matrix A
    sc_signal<int> B[N][N];   // Signals for matrix B
    sc_signal<bool> identical; // Signal for the result

    MatrixComparator comparator_inst; // Instance of the MatrixComparator module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), comparator_inst("comparator") {
        // Connect signals to ports
        comparator_inst.clk(clk);
        comparator_inst.rst(rst);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                comparator_inst.A[i][j](A[i][j]);
                comparator_inst.B[i][j](B[i][j]);
            }
        }
        comparator_inst.identical(identical);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrices
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                A[i][j].write(1 + i); // Fill A with values 1 to 4
                B[i][j].write(1 + i); // Fill B with values 1 to 4
            }
        }

        // Apply reset
        rst.write(true);
        wait(2, SC_NS);
        rst.write(false);

        // Wait for the comparison to complete
        wait(2, SC_NS);

        // Check the result
        if (identical.read()) {
            cout << "Matrices are identical" << endl;
        } else {
            cout << "Matrices are not identical" << endl;
        }

        // Modify B to make matrices different
        B[0][0].write(99); // Change the first element of B

        // Wait for the comparison to complete
        wait(2, SC_NS);

        // Check the result again
        if (identical.read()) {
            cout << "Matrices are identical" << endl;
        } else {
            cout << "Matrices are not identical" << endl;
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
