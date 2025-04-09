
#include <systemc.h>

static const int N = 4;
static const int SIZE = N * N;


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Internal signals to hold the matrix and row/column indices
    sc_signal<int> matrix[N][N];
    sc_signal<int> sorted_matrix[N][N];
    sc_signal<int> row_index;
    sc_signal<int> col_index;
    sc_signal<int> inner_col_index;

    SC_CTOR(RowWiseSorter) {
        SC_THREAD(sort_rows);
        sensitive << clk.pos();
        dont_initialize();

        SC_THREAD(check_done);
        sensitive << clk.pos();
        dont_initialize();
    }

    void sort_rows() {
        while (true) {
            wait(start.pos());

            // Copy the matrix to internal signal for sorting
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    matrix[i][j].write(sorted_matrix[i][j].read());
                }
            }

            // Sort each row
            for (row_index.write(0); row_index.read() < N; ++row_index) {
                for (col_index.write(0); col_index.read() < N; ++col_index) {
                    for (inner_col_index.write(0); inner_col_index.read() < N - col_index.read() - 1; ++inner_col_index) {
                        if (matrix[row_index.read()][inner_col_index.read()].read() > matrix[row_index.read()][inner_col_index.read() + 1].read()) {
                            swap(matrix[row_index.read()][inner_col_index.read()], matrix[row_index.read()][inner_col_index.read() + 1]);
                        }
                    }
                }
            }

            // Signal completion
            done.write(true);
            wait(clk.pos()); // Ensure one clock cycle before clearing the flag
            done.write(false);
        }
    }

    void check_done() {
        while (true) {
            if (done.read()) {
                wait(clk.pos()); // Wait for next clock cycle
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
