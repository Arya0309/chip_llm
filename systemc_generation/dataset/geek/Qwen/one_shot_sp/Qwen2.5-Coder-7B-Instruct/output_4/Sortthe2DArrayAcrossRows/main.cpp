
#include <systemc.h>

// Define constants for matrix dimensions
static const int N = 4;
static const int SIZE = N * N;

// Module to sort a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    // Input ports for matrix data
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_in<int> matrix[SIZE];
    
    // Output port for sorted matrix data
    sc_out<int> sorted_matrix[SIZE];
    sc_out<bool> done;

    // Internal signal to indicate when sorting is complete
    sc_signal<bool> sorting_complete;

    // Constructor for RowWiseSorter module
    SC_CTOR(RowWiseSorter) {
        // Initialize the sorting_complete signal
        sorting_complete.initialize(false);

        // Always process when clock edge is rising and start is true
        SC_METHOD(sort_matrix);
        sensitive << clk.pos();
        dont_initialize();

        // Process when sorting is complete
        SC_THREAD(check_done);
        sensitive << sorting_complete;
    }

    // Method to perform row-wise sorting
    void sort_matrix() {
        while (true) {
            wait(clk.pos()); // Wait for clock edge
            if (start.read()) {
                // Copy matrix data to a local array for sorting
                int m[SIZE];
                for (int i = 0; i < SIZE; i++) {
                    m[i] = matrix[i].read();
                }

                // Perform bubble sort row-wise
                for (int i = 0; i < N; i++) {
                    for (int k = 0; k < N - 1; k++) {
                        if (m[i * N + k] > m[i * N + k + 1]) {
                            int temp = m[i * N + k];
                            m[i * N + k] = m[i * N + k + 1];
                            m[i * N + k + 1] = temp;
                        }
                    }
                }

                // Write sorted matrix back to output port
                for (int i = 0; i < SIZE; i++) {
                    sorted_matrix[i].write(m[i]);
                }

                // Signal that sorting is complete
                sorting_complete.write(true);
            }
        }
    }

    // Thread to check when sorting is complete
    void check_done() {
        while (true) {
            wait(sorting_complete.pos());
            done.write(true); // Indicate sorting is complete
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
