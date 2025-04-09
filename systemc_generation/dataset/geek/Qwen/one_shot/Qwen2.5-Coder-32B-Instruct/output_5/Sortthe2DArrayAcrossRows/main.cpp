
#include <systemc.h>

// Define a constant for the size of the matrix
const int ROWS = 4;
const int COLS = 4;

// Module to sort a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;       // Clock signal
    sc_in<bool> start;     // Start signal to begin sorting
    sc_out<bool> done;      // Done signal indicating completion

    // Internal storage for the matrix
    int matrix[ROWS][COLS];

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Register the process that performs the sorting
        SC_METHOD(sort_matrix);
        sensitive << clk.pos(); // Sensitive to the positive edge of the clock
        dont_initialize();    // Do not initialize immediately
    }

    // Method to sort the matrix
    void sort_matrix() {
        static bool started = false;
        static bool completed = false;

        if (start.read() && !started) {
            started = true;
            completed = false;

            // Initialize the matrix with sample data
            int m[ROWS][COLS] = {{9, 8, 7, 1},
                                 {7, 3, 0, 2},
                                 {9, 5, 3, 2},
                                 {6, 3, 1, 2}};

            // Copy the matrix to the internal storage
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    matrix[i][j] = m[i][j];
                }
            }

            // Sort each row using bubble sort
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

            // Mark the operation as completed
            completed = true;
        }

        // Set the done signal based on the completion status
        done.write(completed);
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
