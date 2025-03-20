
#include <systemc.h>

// Module to sort a 2D matrix row-wise
SC_MODULE(MatrixSorter) {
    // Parameters for matrix dimensions
    static const int ROWS = 4;
    static const int COLS = 4;

    // Input and output ports for matrix
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> reset;        // Reset signal
    sc_in<sc_uint<32>> start; // Start signal to begin sorting
    sc_out<sc_uint<32>> done; // Done signal indicating sorting completion

    // Internal storage for the matrix
    sc_uint<32> matrix[ROWS][COLS];

    // Internal state variables
    sc_uint<32> current_row;
    sc_uint<32> current_col;
    sc_uint<32> inner_col;
    bool sorting;

    // Constructor
    SC_CTOR(MatrixSorter) {
        // Initialization process
        SC_METHOD(initialize);
        sensitive << reset;

        // Sorting process
        SC_METHOD(sort_matrix);
        sensitive << clk.pos();

        // Output process
        SC_METHOD(print_matrix);
        sensitive << done;
    }

    // Method to initialize the matrix and state variables
    void initialize() {
        // Initialize matrix with predefined values
        matrix[0][0] = 9; matrix[0][1] = 8; matrix[0][2] = 7; matrix[0][3] = 1;
        matrix[1][0] = 7; matrix[1][1] = 3; matrix[1][2] = 0; matrix[1][3] = 2;
        matrix[2][0] = 9; matrix[2][1] = 5; matrix[2][2] = 3; matrix[2][3] = 2;
        matrix[3][0] = 6; matrix[3][1] = 3; matrix[3][2] = 1; matrix[3][3] = 2;

        // Initialize state variables
        current_row = 0;
        current_col = 0;
        inner_col = 0;
        sorting = false;
        done.write(0);
    }

    // Method to perform the sorting
    void sort_matrix() {
        if (reset.read()) {
            sorting = false;
            done.write(0);
        } else if (start.read() && !sorting) {
            sorting = true;
        }

        if (sorting) {
            if (inner_col < COLS - current_col - 1) {
                if (matrix[current_row][inner_col] > matrix[current_row][inner_col + 1]) {
                    // Swap elements
                    sc_uint<32> temp = matrix[current_row][inner_col];
                    matrix[current_row][inner_col] = matrix[current_row][inner_col + 1];
                    matrix[current_row][inner_col + 1] = temp;
                }
                inner_col++;
            } else {
                inner_col = 0;
                current_col++;
                if (current_col >= COLS) {
                    current_col = 0;
                    current_row++;
                    if (current_row >= ROWS) {
                        sorting = false;
                        done.write(1);
                    }
                }
            }
        }
    }

    // Method to print the sorted matrix
    void print_matrix() {
        if (done.read()) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    cout << matrix[i][j] << " ";
                }
                cout << endl;
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;             // Clock signal
    sc_signal<bool> reset;    // Reset signal
    sc_signal<sc_uint<32>> start; // Start signal
    sc_signal<sc_uint<32>> done;  // Done signal

    MatrixSorter sorter_inst; // Instance of the MatrixSorter module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), sorter_inst("sorter") {
        // Connect signals to ports
        sorter_inst.clk(clk);
        sorter_inst.reset(reset);
        sorter_inst.start(start);
        sorter_inst.done(done);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Reset the system
        reset.write(1);
        wait(2, SC_NS);
        reset.write(0);

        // Start the sorting process
        start.write(1);
        wait(1, SC_NS);
        start.write(0);

        // Wait for sorting to complete
        wait(done.posedge_event());

        // Print success message
        cout << "Matrix sorted successfully." << endl;

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
