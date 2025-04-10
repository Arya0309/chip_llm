#include <systemc.h>

// Module to perform row-wise sorting on a 4x4 matrix
SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Signal to start sorting
    sc_out<bool> done;    // Signal to indicate sorting is complete

    // Internal storage for the 4x4 matrix
    int mat[4][4];

    SC_CTOR(RowWiseSorter) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
    }

    // Process: Wait for 'start', sort the matrix row-wise, then signal 'done'
    void sort_process() {
        while (true) {
            wait(); // Wait for a clock cycle
            if (start.read() == true) {
                rowWiseSort();  // Perform row-wise sorting
                done.write(true);
                wait();         // Wait for one cycle to signal completion
                done.write(false);
            }
        }
    }

    // Bubble sort for each row of the matrix
    void rowWiseSort() {
        const int r = 4;
        const int c = 4;
        for (int i = 0; i < r; i++) {
            // Bubble sort for row i
            for (int j = 0; j < c; j++) {
                for (int k = 0; k < c - j - 1; k++) {
                    if (mat[i][k] > mat[i][k + 1]) {
                        // Swap the elements
                        int temp = mat[i][k];
                        mat[i][k] = mat[i][k + 1];
                        mat[i][k + 1] = temp;
                    }
                }
            }
        }
    }

    // Helper method to load an input matrix into the module
    void load_matrix(const int input[4][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat[i][j] = input[i][j];
            }
        }
    }

    // Helper method to read the sorted matrix from the module
    void read_matrix(int output[4][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                output[i][j] = mat[i][j];
            }
        }
    }
};

// Testbench Module: Sets up the matrix, triggers sorting, and verifies output
SC_MODULE(Testbench) {
    sc_clock clk;          // Clock signal for synchronization
    sc_signal<bool> start; // Signal to trigger sorting
    sc_signal<bool> done;  // Signal indicating sorting completion

    // Instance of the RowWiseSorter module
    RowWiseSorter* sorter;

    SC_CTOR(Testbench) : clk("clk", 1, SC_NS) {
        sorter = new RowWiseSorter("RowWiseSorter");
        sorter->clk(clk);
        sorter->start(start);
        sorter->done(done);

        SC_THREAD(run_tests);
    }

    // Process: Run the test, compare sorted matrix with expected output
    void run_tests() {
        // Define the unsorted input matrix (same as the C++ code)
        int matrix_in[4][4] = {
            {9, 8, 7, 1},
            {7, 3, 0, 2},
            {9, 5, 3, 2},
            {6, 3, 1, 2}
        };

        // Expected sorted matrix
        int expected[4][4] = {
            {1, 7, 8, 9},
            {0, 2, 3, 7},
            {2, 3, 5, 9},
            {1, 2, 3, 6}
        };

        // Load the input matrix into the sorter
        sorter->load_matrix(matrix_in);

        // Trigger the sorting process
        start.write(true);
        wait(1, SC_NS);
        start.write(false);

        // Wait until the sorter indicates that sorting is done
        while (done.read() != true) {
            wait(1, SC_NS);
        }

        // Retrieve the sorted matrix
        int matrix_out[4][4];
        sorter->read_matrix(matrix_out);

        // Assert that each element of the sorted matrix is as expected
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                assert(matrix_out[i][j] == expected[i][j]);
                std::cout << "Row " << i << ", Col " << j 
                          << " sorted value: " << matrix_out[i][j] << std::endl;
            }
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // End simulation
    }
};

// sc_main: Entry point for the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("Testbench"); // Instantiate the testbench
    sc_start();                // Start the simulation
    return 0;
}
