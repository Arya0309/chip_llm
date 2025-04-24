
#include <systemc.h>

// Module that implements the matrix sorting algorithm

SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk; // Clock signal
    sc_in<bool> start; // Start signal
    sc_out<bool> done; // Done signal
    sc_in<int>* m; // Input matrix
    sc_in<int> r; // Number of rows
    sc_in<int> c; // Number of columns

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Process to perform sorting
        SC_METHOD(SortRows);
        sensitive << start >> m >> r >> c;
    }

    // Method to sort the rows of the matrix
    void SortRows() {
        // Loop over the rows of the matrix
        for (int i = 0; i < r; i++) {
            // Loop over the columns of the current row
            for (int j = 0; j < c; j++) {
                // Loop over the remaining columns of the current row
                for (int k = 0; k < c - j - 1; k++) {
                    // Check if the element at this position is greater than the element at the next position
                    if (m[i][j] > m[i][j + 1]) {
                        // Swap the two elements
                        int temp = m[i][j];
                        m[i][j] = m[i][j + 1];
                        m[i][j + 1] = temp;
                    }
                }
            }
        }
    }
};

// Driver code for the module
int main() {
    int m[][4] = {{9, 8, 7, 1},
                    {7, 3, 0, 2},
                    {9, 5, 3, 2},
                    {6, 3, 1, 2}};
    int r = sizeof(m) / sizeof(m[0]);
    int c = sizeof(m[0]) / sizeof(m[0][0]);
    // Create an instance of the RowWiseSorter module 
    RowWiseSorter sort_matrix(clk, start, done, m, r, c);
    // Start the sorting process
    start = 1;
    // Print the sorted matrix
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}


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
