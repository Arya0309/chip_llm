
#include <systemc.h>

// Module to sort a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;  // Clock input
    sc_in<bool> start;  // Start signal input
    sc_out<bool> done;  // Done signal output
    sc_in<int> m;  // Input matrix
    sc_out<int> m_sorted;  // Output sorted matrix

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Process to sort the matrix
        SC_METHOD(sortRowWise);
        sensitive << clk.pos();
    }

    void sortRowWise() {
        sc_int<32> i = 0;  // row index
        sc_int<32> j = 0;  // column index
        sc_int<32> r = m.size() / sizeof(m[0]);  // number of rows
        sc_int<32> c = m[0].size();  // number of columns

        // Loop over each row of the matrix
        for (i = 0; i < r; i++) {
            // Loop over each column of the current row
            for (j = 0; j < c; j++) {
                // Loop over each element of the current column
                for (sc_int<32> k = 0; k < c - j - 1; k++) {
                    // Compare adjacent elements and swap if necessary
                    if (m[i][k] > m[i][k + 1]) {
                        swap(m[i][k], m[i][k + 1]);
                    }
                }
            }
        }

        // Print the sorted matrix
        for (i = 0; i < r; i++) {
            for (j = 0; j < c; j++) {
                cout << m_sorted[i][j] << " ";
                if (j == c - 1) {
                    cout << "\n";
                }
            }
            if (i == r - 1) {
                cout << "\n";
            }
        }

        done.write(true);  // Set done signal to true
    }

    // Method to swap two integers
    void swap(int *xp, int *yp) {
        int temp = *xp;
        *xp = *yp;
        *yp = temp;
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
