
#include <systemc.h>

// Module that sorts a 2D matrix row-wise
SC_MODULE(MatrixSorter) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> reset;        // Reset signal
    sc_in<sc_uint<32>> m[4][4]; // Input matrix
    sc_out<sc_uint<32>> sorted_m[4][4]; // Output sorted matrix

    // Internal variables
    int r = 4; // Number of rows
    int c = 4; // Number of columns
    sc_uint<32> matrix[4][4];
    sc_uint<32> sorted_matrix[4][4];

    // Constructor
    SC_CTOR(MatrixSorter) {
        SC_METHOD(sort_matrix);
        sensitive << clk.posedge_event();
        dont_initialize();
    }

    // Method to sort the matrix row-wise
    void sort_matrix() {
        if (reset.read()) {
            // Initialize the sorted matrix with zeros
            for (int i = 0; i < r; ++i) {
                for (int j = 0; j < c; ++j) {
                    sorted_matrix[i][j] = 0;
                }
            }
        } else {
            // Copy input matrix to internal matrix
            for (int i = 0; i < r; ++i) {
                for (int j = 0; j < c; ++j) {
                    matrix[i][j] = m[i][j].read();
                }
            }

            // Bubble sort each row
            for (int i = 0; i < r; ++i) {
                for (int j = 0; j < c; ++j) {
                    for (int k = 0; k < c - j - 1; ++k) {
                        if (matrix[i][k] > matrix[i][k + 1]) {
                            // Swap elements
                            sc_uint<32> temp = matrix[i][k];
                            matrix[i][k] = matrix[i][k + 1];
                            matrix[i][k + 1] = temp;
                        }
                    }
                }
            }

            // Copy sorted matrix to output
            for (int i = 0; i < r; ++i) {
                for (int j = 0; j < c; ++j) {
                    sorted_matrix[i][j] = matrix[i][j];
                    sorted_m[i][j].write(sorted_matrix[i][j]);
                }
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;             // Clock signal
    sc_signal<bool> reset;    // Reset signal
    sc_signal<sc_uint<32>> m[4][4]; // Input matrix
    sc_signal<sc_uint<32>> sorted_m[4][4]; // Output sorted matrix

    MatrixSorter sorter_inst; // Instance of the MatrixSorter module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), sorter_inst("sorter") {
        // Connect signals to ports
        sorter_inst.clk(clk);
        sorter_inst.reset(reset);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                sorter_inst.m[i][j](m[i][j]);
                sorter_inst.sorted_m[i][j](sorted_m[i][j]);
            }
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrix
        m[0][0] = 9; m[0][1] = 8; m[0][2] = 7; m[0][3] = 1;
        m[1][0] = 7; m[1][1] = 3; m[1][2] = 0; m[1][3] = 2;
        m[2][0] = 9; m[2][1] = 5; m[2][2] = 3; m[2][3] = 2;
        m[3][0] = 6; m[3][1] = 3; m[3][2] = 1; m[3][3] = 2;

        // Reset signal
        reset = true;
        wait(2, SC_NS);
        reset = false;

        // Wait for sorting to complete
        wait(50, SC_NS);

        // Print sorted matrix
        cout << "Sorted Matrix:" << endl;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                cout << sorted_m[i][j].read() << " ";
            }
            cout << endl;
        }

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
