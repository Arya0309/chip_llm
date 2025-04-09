
#include <systemc.h>


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Define the matrix dimensions
    const static int ROWS = 4;
    const static int COLS = 4;

    // Input and output matrix
    sc_signal<int> matrix[ROWS][COLS];
    sc_signal<int> sorted_matrix[ROWS][COLS];

    // Internal signals for state management
    sc_signal<int> row_index;
    sc_signal<int> col_index;
    sc_signal<int> compare_index;
    sc_signal<bool> sorting_done;

    // Constructor
    SC_CTOR(RowWiseSorter) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
        async_reset_signal_is(clk, false);

        SC_METHOD(print_sorted_matrix);
        sensitive << sorting_done;
    }

    void sort_process() {
        while (true) {
            wait(); // Wait for clock edge

            if (start.read()) {
                row_index.write(0);
                col_index.write(0);
                compare_index.write(0);
                sorting_done.write(false);

                for (int i = 0; i < ROWS; ++i) {
                    for (int j = 0; j < COLS; ++j) {
                        sorted_matrix[i][j].write(matrix[i][j].read());
                    }
                }

                for (int i = 0; i < ROWS; ++i) {
                    for (int j = 0; j < COLS; ++j) {
                        for (int k = 0; k < COLS - j - 1; ++k) {
                            if (sorted_matrix[i][k].read() > sorted_matrix[i][k + 1].read()) {
                                int temp = sorted_matrix[i][k].read();
                                sorted_matrix[i][k].write(sorted_matrix[i][k + 1].read());
                                sorted_matrix[i][k + 1].write(temp);
                            }
                        }
                    }
                }

                sorting_done.write(true);
            }
        }
    }

    void print_sorted_matrix() {
        if (sorting_done.read()) {
            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {
                    cout << sorted_matrix[i][j].read() << " ";
                }
                cout << endl;
            }
            done.write(true);
        } else {
            done.write(false);
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
