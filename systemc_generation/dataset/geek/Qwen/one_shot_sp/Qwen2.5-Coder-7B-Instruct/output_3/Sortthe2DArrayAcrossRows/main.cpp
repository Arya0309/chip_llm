
#include <systemc.h>

// Constants for matrix size
static const int N = 4;
static const int SIZE = N * N;

// Module that sorts a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    // Input ports for matrix data
    sc_in<bool> clk;
    sc_in<bool> start;
    // Output port for indication of completion
    sc_out<bool> done;

    // Internal signals to hold matrix data
    sc_signal<int> matrix[N][N];
    sc_signal<bool> sort_complete;

    SC_CTOR(RowWiseSorter) {
        // Sensitive edge detection for clock and start signal
        SC_THREAD(sort_matrix);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(check_done);
        sensitive << sort_complete;
    }

    // Method to sort the matrix row-wise
    void sort_matrix() {
        while (true) {
            wait();  // Wait for rising edge of clock
            if (start.read()) {
                // Copy input matrix data into internal signals
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        matrix[i][j].write(matrix[i][j].read());
                    }
                }

                // Sort each row of the matrix
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        for (int k = 0; k < N - j - 1; k++) {
                            if (matrix[i][k].read() > matrix[i][k + 1].read()) {
                                swap(matrix[i][k].read(), matrix[i][k + 1].read());
                            }
                        }
                    }
                }

                // Indicate that sorting is complete
                sort_complete.write(true);

                // Wait for next cycle before accepting new start signal
                wait();
                sort_complete.write(false);
            }
        }
    }

    // Method to check if sorting is complete and set done signal
    void check_done() {
        done.write(sort_complete.read());
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
