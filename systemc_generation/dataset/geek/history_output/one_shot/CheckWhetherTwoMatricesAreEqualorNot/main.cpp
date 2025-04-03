
#include <systemc.h>

// Module to compare two matrices
SC_MODULE(MatrixComparator) {
    sc_in<bool> clk;            // Clock signal
    sc_in<bool> rst_n;          // Active-low reset signal
    sc_in<int> matrixA[4][4];  // Input ports for matrix A
    sc_in<int> matrixB[4][4];  // Input ports for matrix B
    sc_out<bool> matricesEqual; // Output port indicating if matrices are equal

    bool result; // Internal variable to store comparison result

    // Constructor
    SC_CTOR(MatrixComparator) {
        // Process to perform matrix comparison
        SC_METHOD(compare_matrices);
        sensitive << clk.posedge_event() << rst_n.negedge_event();
    }

    // Method to compare the matrices
    void compare_matrices() {
        if (!rst_n) {
            result = false; // Reset result on reset
        } else {
            result = true; // Assume matrices are equal initially
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (matrixA[i][j].read() != matrixB[i][j].read()) {
                        result = false; // Set result to false if any element differs
                        break; // No need to check further
                    }
                }
                if (!result) break; // Break outer loop early if result is false
            }
        }
        matricesEqual.write(result); // Write the result to the output port
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;                  // Clock signal
    sc_signal<bool> rst_n;         // Active-low reset signal
    sc_signal<int> matrixA[4][4];  // Signals for matrix A
    sc_signal<int> matrixB[4][4];  // Signals for matrix B
    sc_signal<bool> matricesEqual; // Signal indicating if matrices are equal

    MatrixComparator comparator_inst; // Instance of the MatrixComparator module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS, 0.5), comparator_inst("comparator") {
        // Connect signals to ports
        comparator_inst.clk(clk);
        comparator_inst.rst_n(rst_n);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                comparator_inst.matrixA[i][j](matrixA[i][j]);
                comparator_inst.matrixB[i][j](matrixB[i][j]);
            }
        }
        comparator_inst.matricesEqual(matricesEqual);

        // Process to initialize and run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrices A and B
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                matrixA[i][j] = i * 4 + j + 1; // Fill matrix A with unique values
                matrixB[i][j] = i * 4 + j + 1; // Fill matrix B with the same values
            }
        }

        // Apply reset
        rst_n = false;
        wait(1, SC_NS);
        rst_n = true;
        wait(1, SC_NS);

        // Check if matrices are equal
        wait(1, SC_NS); // Wait for one clock cycle for the result to be valid
        if (matricesEqual.read())
            cout << "Matrices are identical" << endl;
        else
            cout << "Matrices are not identical" << endl;

        // Modify matrix B to make it different
        matrixB[3][3] = 0; // Change one element in matrix B
        wait(1, SC_NS); // Wait for one clock cycle for the result to be valid
        if (matricesEqual.read())
            cout << "Matrices are identical" << endl;
        else
            cout << "Matrices are not identical" << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
