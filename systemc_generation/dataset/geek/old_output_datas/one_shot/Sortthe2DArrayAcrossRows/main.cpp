
#include <systemc.h>

// Module to sort a 2D matrix row-wise
SC_MODULE(MatrixSorter) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> reset;        // Reset signal
    sc_in<bool> start;        // Start signal to initiate sorting
    sc_out<bool> done;        // Done signal indicating sorting completion

    // Fixed-size matrix input and output ports
    sc_in<int> matrix_in[4][4]; // Input matrix
    sc_out<int> matrix_out[4][4]; // Output matrix

    int matrix[4][4];         // Internal matrix storage
    bool is_done;              // Flag to indicate sorting completion

    // Constructor
    SC_CTOR(MatrixSorter) {
        // Process to handle sorting
        SC_METHOD(sort_matrix);
        sensitive << clk.pos();
        dont_initialize();

        // Process to handle reset
        SC_METHOD(reset_process);
        sensitive << reset;
    }

    // Method to reset the sorter
    void reset_process() {
        if (reset.read()) {
            is_done = false;
            done.write(false);
            // Clear the matrix if needed
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    matrix[i][j] = 0;
                    matrix_out[i][j].write(0);
                }
            }
        }
    }

    // Method to sort the matrix row-wise
    void sort_matrix() {
        if (start.read() && !is_done) {
            // Copy input matrix to internal storage
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    matrix[i][j] = matrix_in[i][j].read();
                }
            }

            // Perform row-wise sorting using bubble sort
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 4 - j - 1; ++k) {
                        if (matrix[i][k] > matrix[i][k + 1]) {
                            // Swap elements
                            int temp = matrix[i][k];
                            matrix[i][k] = matrix[i][k + 1];
                            matrix[i][k + 1] = temp;
                        }
                    }
                }
            }

            // Write sorted matrix to output ports
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    matrix_out[i][j].write(matrix[i][j]);
                }
            }

            // Indicate sorting is done
            done.write(true);
            is_done = true;
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;                  // Clock signal
    sc_signal<bool> reset;          // Reset signal
    sc_signal<bool> start;          // Start signal
    sc_signal<bool> done;           // Done signal

    sc_signal<int> matrix_in[4][4]; // Input matrix signals
    sc_signal<int> matrix_out[4][4];// Output matrix signals

    MatrixSorter sorter_inst;       // Instance of the MatrixSorter module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), sorter_inst("sorter") {
        // Connect signals to ports
        sorter_inst.clk(clk);
        sorter_inst.reset(reset);
        sorter_inst.start(start);
        sorter_inst.done(done);

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                sorter_inst.matrix_in[i][j](matrix_in[i][j]);
                sorter_inst.matrix_out[i][j](matrix_out[i][j]);
            }
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrix with test values
        matrix_in[0][0] = 9; matrix_in[0][1] = 8; matrix_in[0][2] = 7; matrix_in[0][3] = 1;
        matrix_in[1][0] = 7; matrix_in[1][1] = 3; matrix_in[1][2] = 0; matrix_in[1][3] = 2;
        matrix_in[2][0] = 9; matrix_in[2][1] = 5; matrix_in[2][2] = 3; matrix_in[2][3] = 2;
        matrix_in[3][0] = 6; matrix_in[3][1] = 3; matrix_in[3][2] = 1; matrix_in[3][3] = 2;

        // Reset the sorter
        reset = true;
        wait(1, SC_NS);
        reset = false;
        wait(1, SC_NS);

        // Start sorting
        start = true;
        wait(1, SC_NS);
        start = false;

        // Wait until sorting is done
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Print the sorted matrix
        cout << "Sorted Matrix:" << endl;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                cout << matrix_out[i][j].read() << " ";
            }
            cout << endl;
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
