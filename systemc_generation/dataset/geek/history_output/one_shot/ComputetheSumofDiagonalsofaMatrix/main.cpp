
#include <systemc.h>

// Module to compute diagonal sums of a matrix
SC_MODULE(DiagonalSums) {
    sc_in<int> matrix[100][100];  // Input ports for matrix elements
    sc_in<int> size;              // Input port for matrix size
    sc_out<int> principal_sum;     // Output port for principal diagonal sum
    sc_out<int> secondary_sum;   // Output port for secondary diagonal sum

    // Constructor
    SC_CTOR(DiagonalSums) {
        // Process to compute diagonal sums
        SC_METHOD(compute_sums);
        sensitive << size;
    }

    // Method to compute the sums of the diagonals
    void compute_sums() {
        int n = size.read();
        int principal = 0, secondary = 0;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j)
                    principal += matrix[i][j].read();

                if ((i + j) == (n - 1))
                    secondary += matrix[i][j].read();
            }
        }

        principal_sum.write(principal);
        secondary_sum.write(secondary);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> matrix[100][100]; // Signals for matrix elements
    sc_signal<int> size;             // Signal for matrix size
    sc_signal<int> principal_sum;    // Signal for principal diagonal sum
    sc_signal<int> secondary_sum;  // Signal for secondary diagonal sum

    DiagonalSums diag_sums_inst; // Instance of the DiagonalSums module

    // Constructor
    SC_CTOR(Testbench) : diag_sums_inst("diag_sums") {
        // Connect signals to ports
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                diag_sums_inst.matrix[i][j](matrix[i][j]);
            }
        }
        diag_sums_inst.size(size);
        diag_sums_inst.principal_sum(principal_sum);
        diag_sums_inst.secondary_sum(secondary_sum);

        // Process to initialize matrix and run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrix
        matrix[0][0] = 1; matrix[0][1] = 2; matrix[0][2] = 3; matrix[0][3] = 4;
        matrix[1][0] = 5; matrix[1][1] = 6; matrix[1][2] = 7; matrix[1][3] = 8;
        matrix[2][0] = 1; matrix[2][1] = 2; matrix[2][2] = 3; matrix[2][3] = 4;
        matrix[3][0] = 5; matrix[3][1] = 6; matrix[3][2] = 7; matrix[3][3] = 8;
        size = 4;

        // Wait for the computation to complete
        wait(1, SC_NS);

        // Print results
        cout << "Principal Diagonal: " << principal_sum.read() << endl;
        cout << "Secondary Diagonal: " << secondary_sum.read() << endl;

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
