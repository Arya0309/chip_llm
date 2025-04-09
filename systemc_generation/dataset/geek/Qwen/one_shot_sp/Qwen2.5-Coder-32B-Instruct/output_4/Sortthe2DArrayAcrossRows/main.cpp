
#include <systemc.h>

// Use static const for constants instead of #define
static const int ROWS = 4;
static const int COLS = 4;
static const int SIZE = ROWS * COLS;

// Module that sorts a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    // Input clock signal
    sc_in<bool> clk;
    // Start signal to initiate sorting
    sc_in<bool> start;
    // Done signal to indicate completion
    sc_out<bool> done;
    // Input matrix flattened as 1D array
    sc_in<int> matrix_in[SIZE];
    // Output matrix flattened as 1D array
    sc_out<int> matrix_out[SIZE];

    // Internal storage for the matrix
    int matrix[ROWS][COLS];
    // Flag to track sorting completion
    bool sorting_done;

    SC_CTOR(RowWiseSorter) {
        // Initialize the sorting flag
        sorting_done = false;
        // Register a method to handle sorting
        SC_METHOD(sort_matrix);
        sensitive << clk.pos();
        dont_initialize();
    }

    // Method to sort the matrix row-wise
    void sort_matrix() {
        if (start.read()) {
            // Copy input matrix to internal storage
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    matrix[i][j] = matrix_in[i * COLS + j].read();
                }
            }

            // Perform row-wise sorting using bubble sort
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    for (int k = 0; k < COLS - j - 1; k++) {
                        if (matrix[i][k] > matrix[i][k + 1]) {
                            // Swap elements
                            int temp = matrix[i][k];
                            matrix[i][k] = matrix[i][k + 1];
                            matrix[i][k + 1] = temp;
                        }
                    }
                }
            }

            // Write sorted matrix to output
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    matrix_out[i * COLS + j].write(matrix[i][j]);
                }
            }

            // Set done signal to true
            sorting_done = true;
            done.write(true);
        } else {
            // Keep done signal false when not sorting
            done.write(false);
        }
    }
};

// Example usage


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
