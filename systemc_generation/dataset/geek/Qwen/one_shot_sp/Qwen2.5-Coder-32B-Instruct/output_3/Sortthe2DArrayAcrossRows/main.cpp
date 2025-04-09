
#include <systemc.h>

// Use static const for constants instead of #define
static const int ROWS = 4;
static const int COLS = 4;
static const int SIZE = ROWS * COLS;

// Module that performs row-wise sorting of a matrix

SC_MODULE(RowWiseSorter) {
    // Clock and control signals
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Input and output ports for the matrix (flattened as 1D arrays)
    sc_in<int> matrix_in[SIZE];
    sc_out<int> matrix_out[SIZE];

    // Internal storage for the matrix
    int matrix[ROWS][COLS];

    // State variables
    bool processing;
    int row, col, k;

    SC_CTOR(RowWiseSorter) {
        // Register the process for sorting
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        // Initialize state variables
        processing = false;
        row = 0;
        col = 0;
        k = 0;
        done.write(false);

        // Register the process for handling start signal
        SC_METHOD(start_process);
        sensitive << start;
        dont_initialize();
    }

    // Process to handle the start signal
    void start_process() {
        if (start.read()) {
            // Load matrix from input ports
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    matrix[i][j] = matrix_in[i * COLS + j]->read();
                }
            }
            // Reset state variables and start processing
            processing = true;
            row = 0;
            col = 0;
            k = 0;
            done.write(false);
        }
    }

    // Process to perform row-wise sorting
    void sort_process() {
        if (processing) {
            // Perform bubble sort on the current row
            if (k < COLS - col - 1) {
                if (matrix[row][k] > matrix[row][k + 1]) {
                    // Swap elements
                    int temp = matrix[row][k];
                    matrix[row][k] = matrix[row][k + 1];
                    matrix[row][k + 1] = temp;
                }
                k++;
            } else {
                k = 0;
                col++;
                if (col >= COLS) {
                    col = 0;
                    row++;
                    if (row >= ROWS) {
                        // Sorting complete, write output and set done signal
                        for (int i = 0; i < ROWS; i++) {
                            for (int j = 0; j < COLS; j++) {
                                matrix_out[i * COLS + j]->write(matrix[i][j]);
                            }
                        }
                        processing = false;
                        done.write(true);
                    }
                }
            }
        }
    }
};

// Example usage in a testbench


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
