
#include <systemc.h>


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;         // Clock signal
    sc_in<bool> start;       // Start signal to initiate sorting
    sc_out<bool> done;       // Done signal indicating completion

    // Internal storage for the matrix
    int matrix[4][4];
    bool is_done;

    // Constructor
    SC_CTOR(RowWiseSorter) {
        is_done = false;
        SC_METHOD(sort_matrix);
        sensitive << clk.pos();
    }

    // Method to sort the matrix row-wise
    void sort_matrix() {
        if (start.read()) {
            int r = 4;
            int c = 4;
            
            // Perform row-wise sorting using bubble sort
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    for (int k = 0; k < c - j - 1; k++) {
                        if (matrix[i][k] > matrix[i][k + 1]) {
                            // Swap elements
                            int temp = matrix[i][k];
                            matrix[i][k] = matrix[i][k + 1];
                            matrix[i][k + 1] = temp;
                        }
                    }
                }
            }
            
            // Indicate completion
            is_done = true;
            done.write(is_done);
        } else {
            done.write(false);
        }
    }

    // Function to set the initial matrix
    void set_matrix(int m[][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                matrix[i][j] = m[i][j];
            }
        }
        is_done = false;
        done.write(is_done);
    }

    // Function to get the sorted matrix (for testing purposes)
    void get_matrix(int m[][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = matrix[i][j];
            }
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
