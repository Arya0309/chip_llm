
#include <systemc.h>

// Define constants for matrix size
static const int N = 4;
static const int SIZE = N * N;

// Module that sorts a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    // Input ports for matrix (flattened as 1D array)
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_in<int> m[SIZE];
    // Output port for sorted matrix (flattened as 1D array)
    sc_out<int> sorted_m[SIZE];
    // Output signal indicating completion
    sc_out<bool> done;

    // Internal signals
    bool running;
    int sorted[N][N];
    int current_row, current_col, next_col;

    SC_CTOR(RowWiseSorter) {
        // Initialize internal states
        running = false;
        current_row = 0;
        current_col = 0;
        next_col = 0;
        done.initialize(false);

        // Process changes in 'start' signal
        SC_METHOD(start_signal_handler);
        sensitive << start.pos();

        // Process changes in 'clk' signal
        SC_METHOD(sort_matrix);
        sensitive << clk.pos();
    }

    // Method to handle 'start' signal
    void start_signal_handler() {
        if (start.read()) {
            running = true;
            current_row = 0;
            current_col = 0;
            next_col = 0;
        }
    }

    // Method to sort the matrix
    void sort_matrix() {
        if (!running) {
            done.write(false);
            return;
        }

        // Copy input matrix to internal buffer
        for (int i = 0; i < SIZE; i++) {
            sorted[i / N][i % N] = m[i].read();
        }

        // Sort the matrix row-wise
        for (; current_row < N; current_row++) {
            for (current_col = 0; current_col < N - next_col - 1; current_col++) {
                if (sorted[current_row][current_col] > sorted[current_row][current_col + 1]) {
                    int temp = sorted[current_row][current_col];
                    sorted[current_row][current_col] = sorted[current_row][current_col + 1];
                    sorted[current_row][current_col + 1] = temp;
                }
            }
            next_col++;
        }

        // Write sorted matrix back to output port
        for (int i = 0; i < SIZE; i++) {
            sorted_m[i].write(sorted[i / N][i % N]);
        }

        // Set done signal and stop processing
        running = false;
        done.write(true);
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
