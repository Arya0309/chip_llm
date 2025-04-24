
// This code is contributed by Rutvik_56

#include <systemc.h>

// Module that implements RowWiseSorter

SC_MODULE(RowWiseSorter) {
    sc_in<sc_int<4>> a;   // Input 2D matrix
    sc_in<sc_int<4>> r;   // Row count
    sc_in<sc_int<4>> c;   // Column count
    sc_out<sc_int<4>> b;  // Sort matrix

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Latch bubble sort algorithm
        b = a;
        sc_out = c;

        // Counter initialization
        sc_int<4> row_counter = 0;
        sc_int<4> column_counter = 0;
        sc_bool sort_flag = sc_const<bool>(1);
        sc_bool swap_flag = sc_const<bool>(1);

        // Process to sort matrix
        SC_METHOD(sort_rowWise);
        sensitive << a << r << c;
    }

    /* Performs entry-wise sorting on a 2D matrix row-wise */
    void sort_rowWise() {
        // Loop over rows
        if (row_counter < r) {
            // Loop over columns
            for (column_counter = 0; column_counter < c; ++column_counter) {
                // Loop over pairs of elements within a column
                for (swap_flag = sc_const<bool>(1); exchange_flag;
                     swap_flag = sc_const<bool>(0)) {
                    // Swap elements when necessary
                    if (a[row_counter][column_counter] >
                        a[row_counter + 1][column_counter]) {
                        // Replace original element with sorted element
                        a[row_counter + 1][column_counter] =
                           a[row_counter][column_counter];
                        // Set sort flag to indicate swap has occurred
                        swap_flag = sc_const<bool>(1);
                    }
                }

                // Set sort flag to False if no swaps have been performed
                swap_flag = sc_const<bool>(0);
            }

            // Increment row counter and reset column counter
            ++row_counter;
            column_counter = 0;
        }

        // Output sorted matrix
        b = a;
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
