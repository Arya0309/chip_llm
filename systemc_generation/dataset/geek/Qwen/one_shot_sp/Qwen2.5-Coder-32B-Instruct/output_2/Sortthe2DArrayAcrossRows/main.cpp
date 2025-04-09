
#include <systemc.h>

// Define constants for matrix size
static const int ROWS = 4;
static const int COLS = 4;

// Module that performs row-wise sorting of a matrix

SC_MODULE(RowWiseSorter) {
    // Clock signal
    sc_in<bool> clk;
    // Start signal to initiate sorting
    sc_in<bool> start;
    // Done signal to indicate completion of sorting
    sc_out<bool> done;

    // Input matrix (flattened as 1D array)
    sc_in<int> matrix_in[ROWS * COLS];
    // Output matrix (flattened as 1D array)
    sc_out<int> matrix_out[ROWS * COLS];

    // Internal storage for matrix during processing
    int matrix[ROWS][COLS];

    // State variable to track sorting progress
    enum State {IDLE, SORTING, DONE};
    State currentState;

    // Constructor
    SC_CTOR(RowWiseSorter) : currentState(IDLE) {
        // Register process
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        // Initialize done signal
        done.write(false);
    }

    // Process method for sorting
    void sort_process() {
        if (start.read()) {
            currentState = SORTING;
            // Copy input matrix to internal storage
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    matrix[i][j] = matrix_in[i * COLS + j].read();
                }
            }
        }

        if (currentState == SORTING) {
            // Perform row-wise sorting using bubble sort
            static int i = 0, j = 0, k = 0;
            if (i < ROWS) {
                if (j < COLS) {
                    if (k < COLS - j - 1) {
                        if (matrix[i][k] > matrix[i][k + 1]) {
                            // Swap elements
                            int temp = matrix[i][k];
                            matrix[i][k] = matrix[i][k + 1];
                            matrix[i][k + 1] = temp;
                        }
                        k++;
                    } else {
                        k = 0;
                        j++;
                    }
                } else {
                    j = 0;
                    i++;
                }
            } else {
                // Sorting complete, copy sorted matrix to output
                for (int i = 0; i < ROWS; i++) {
                    for (int j = 0; j < COLS; j++) {
                        matrix_out[i * COLS + j].write(matrix[i][j]);
                    }
                }
                currentState = DONE;
                done.write(true);
            }
        }
    }
};



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
