scss
#include <systemc.h>

// Module that sorts a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    // Input ports
    sc_in<int, 2> m; // 2D input matrix
    sc_in<int> r; // Number of rows in the input matrix
    sc_in<int> c; // Number of columns in the input matrix
    
    // Output ports
    sc_out<int, 2> sorted_matrix; // Output matrix
    sc_out<bool> done; // Indicates when the sorting process is complete

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Process to sort the input matrix
        SC_METHOD(sort);
        sensitive << m << r << c;
    }

    // Method to sort the input matrix
    void sort() {
        // Initialize the sorted matrix to zero
        sorted_matrix.write(0);

        // Iterate through each row of the input matrix
        for (int i = 0; i < r; i++) {
            // Iterate through each column of the row
            for (int j = 0; j < c; j++) {
                // Iterate through each element of the column
                for (int k = 0; k < c - j - 1; k++) {
                    // Swap the elements if they are out of order
                    if (m.read(i, j) > m.read(i, k + 1)) {
                        m.write(i, j, m.read(i, k + 1));
                        m.write(i, k + 1, m.read(i, j));
                    }
                }
            }
        }

        // Set the sorted matrix as the output
        sorted_matrix.write(m);

        // Indicate that the sorting process is complete
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
