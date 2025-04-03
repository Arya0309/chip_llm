
#include <systemc.h>

// Module to interchange the first and last rows of a matrix
SC_MODULE(MatrixRowSwapper) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> reset;        // Reset signal
    sc_in<sc_matrix<int>> in_matrix; // Input matrix
    sc_out<sc_matrix<int>> out_matrix; // Output matrix

    // Internal matrix to store the modified matrix
    sc_matrix<int> matrix;

    // Constructor
    SC_CTOR(MatrixRowSwapper) {
        // Process to perform row swapping
        SC_METHOD(swap_rows);
        sensitive << clk.posedge_event();
    }

    // Method to swap the first and last rows
    void swap_rows() {
        if (reset.read()) {
            matrix = in_matrix.read();
        } else {
            int rows = matrix.size();
            for (int i = 0; i < rows; i++) {
                int temp = matrix[0][i];
                matrix[0][i] = matrix[rows - 1][i];
                matrix[rows - 1][i] = temp;
            }
            out_matrix.write(matrix);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;              // Clock signal
    sc_signal<bool> reset;     // Reset signal
    sc_signal<sc_matrix<int>> in_matrix; // Input matrix
    sc_signal<sc_matrix<int>> out_matrix; // Output matrix

    MatrixRowSwapper swapper_inst; // Instance of the MatrixRowSwapper module

    // Constructor
    SC_CTOR(Testbench) : swapper_inst("swapper"), clk("clk", 10, SC_NS) {
        // Connect signals to ports
        swapper_inst.clk(clk);
        swapper_inst.reset(reset);
        swapper_inst.in_matrix(in_matrix);
        swapper_inst.out_matrix(out_matrix);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize the matrix
        sc_matrix<int> init_matrix = {{8, 9, 7, 6},
                                      {4, 7, 6, 5},
                                      {3, 2, 1, 8},
                                      {9, 9, 7, 7}};
        in_matrix.write(init_matrix);
        reset.write(true);
        wait(1, SC_NS);
        reset.write(false);
        wait(1, SC_NS);

        // Print the interchanged matrix
        sc_matrix<int> result_matrix = out_matrix.read();
        for (int i = 0; i < result_matrix.size(); i++) {
            for (int j = 0; j < result_matrix[i].size(); j++) {
                cout << result_matrix[i][j] << " ";
            }
            cout << endl;
        }

        sc_stop(); // Stop the simulation
    }
};

// Custom type definition for a 4x4 matrix
typedef std::vector<std::vector<int>> sc_matrix;

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
