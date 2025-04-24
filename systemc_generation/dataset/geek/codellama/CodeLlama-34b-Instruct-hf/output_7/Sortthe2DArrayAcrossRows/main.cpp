
#include <systemc.h>

// Constants
const int NUM_ROWS = 4; // Number of rows in the matrix
const int NUM_COLS = 4; // Number of columns in the matrix

// Module that sorts a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    sc_in<int> r; // Input port for the number of rows
    sc_in<int> c; // Input port for the number of columns
    sc_in<int> m[NUM_ROWS][NUM_COLS]; // Input port for the 2D matrix
    sc_out<int> sorted_m[NUM_ROWS][NUM_COLS]; // Output port for the sorted 2D matrix

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Process to perform the sort
        SC_METHOD(sort);
        sensitive << r << c << m;
    }

    // Method to sort the 2D matrix
    void sort() {
        // Initialize the sorted matrix
        sorted_m[0][0] = 0;
        sorted_m[1][0] = 0;
        sorted_m[2][0] = 0;
        sorted_m[3][0] = 0;

        // Iterate over the rows of the matrix
        for (int i = 0; i < r; i++) {
            // Iterate over the columns of the current row
            for (int j = 1; j < c; j++) {
                // Iterate over the previous columns
                for (int k = 0; k < j; k++) {
                    if (m[i][k] > m[i][k + 1]) {
                        // Swap the values
                        sorted_m[i][k] = m[i][k + 1];
                        sorted_m[i][k + 1] = m[i][k];
                        break;
                    }
                }
            }
        }

        // Print the sorted matrix
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                sc_uint<32> tmp = sorted_m[i][j];
                cout << tmp << " ";
            }
            cout << endl;
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
