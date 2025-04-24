scss
#include <systemc.h>

// Module to sort a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    // Input ports
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_in<int> matrix[4][4];

    // Output ports
    sc_out<bool> done;
    sc_out<int[4][4]> sortedMatrix;

    // Local variables
    int m[4][4];
    int r, c, i, j, k;
    bool sorted = false;

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Read input matrix
        SC_METHOD(readMatrix);
        sensitive << clk << start;
    }

    // Read input matrix and initialize variables
    void readMatrix() {
        if (start.read() == true) {
            // Copy input matrix to local matrix
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    m[i][j] = matrix[i][j];
                }
            }
            // Initialize other variables
            r = 4;
            c = 4;
            i = 0;
            j = 0;
            k = 0;
            sorted = false;
        }
    }

    // Sort the matrix row-wise
    void sortRowWise() {
        // Outer loop: loop through each row of the matrix
        for (i = 0; i < r; i++) {
            // Inner loop: loop through each column of the row
            for (j = 0; j < c; j++) {
                // Check if the value at the current position is greater than the value at the next position
                if (m[i][j] > m[i][k + 1]) {
                    // Swap the values at the current position and the next position
                    int temp = m[i][j];
                    m[i][j] = m[i][k + 1];
                    m[i][k + 1] = temp;
                }
                k++;
            }
        }
    }

    // Print the sorted matrix
    void printSortedMatrix() {
        if (sorted == true) {
            // Print the sorted matrix
            for (i = 0; i < r; i++) {
                for (j = 0; j < c; j++) {
                    sortedMatrix[i][j] = m[i][j];
                    std::cout << m[i][j] << " ";
                }
                std::cout << std::endl;
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
