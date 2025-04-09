
#include <systemc.h>

// Module to sort a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> start;        // Start signal
    sc_out<bool> done;         // Done signal

    static const int ROWS = 4;
    static const int COLS = 4;
    int matrix[ROWS][COLS];

    SC_CTOR(RowWiseSorter) {
        SC_THREAD(sort_matrix);
        sensitive << clk.pos();
        reset_signal_is(start, false);
    }

    void sort_matrix() {
        wait(); // Wait for the first clock cycle

        while (true) {
            wait(); // Wait for the start signal
            if (start.read()) {
                // Initialize matrix (this would typically come from external input)
                matrix[0][0] = 9; matrix[0][1] = 8; matrix[0][2] = 7; matrix[0][3] = 1;
                matrix[1][0] = 7; matrix[1][1] = 3; matrix[1][2] = 0; matrix[1][3] = 2;
                matrix[2][0] = 9; matrix[2][1] = 5; matrix[2][2] = 3; matrix[2][3] = 2;
                matrix[3][0] = 6; matrix[3][1] = 3; matrix[3][2] = 1; matrix[3][3] = 2;

                // Sort each row of the matrix
                for (int i = 0; i < ROWS; i++) {
                    for (int j = 0; j < COLS; j++) {
                        for (int k = 0; k < COLS - j - 1; k++) {
                            if (matrix[i][k] > matrix[i][k + 1]) {
                                int temp = matrix[i][k];
                                matrix[i][k] = matrix[i][k + 1];
                                matrix[i][k + 1] = temp;
                            }
                        }
                    }
                }

                // Indicate completion
                done.write(true);
                wait(); // Hold the done signal for one clock cycle
                done.write(false);
            }
        }
    }
};

// Example testbench


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
