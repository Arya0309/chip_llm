
#include <systemc.h>


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> start;  // Start signal
    sc_out<bool> done;  // Done signal

    sc_in<sc_uint<32>> rows;  // Number of rows in the matrix
    sc_in<sc_uint<32>> cols;  // Number of columns in the matrix
    sc_inout_array<sc_uint<32>> matrix;  // Input/output matrix

    sc_signal<bool> sort_done;  // Internal signal to indicate sorting is done

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Process to handle sorting
        SC_METHOD(sort_matrix);
        sensitive << clk.pos();
        dont_initialize();

        // Process to set done signal
        SC_METHOD(set_done);
        sensitive << sort_done;
    }

    // Method to sort the matrix
    void sort_matrix() {
        if (start.read()) {
            for (int i = 0; i < rows.read(); i++) {
                for (int j = 0; j < cols.read(); j++) {
                    for (int k = 0; k < cols.read() - j - 1; k++) {
                        if (matrix[i][k] > matrix[i][k + 1]) {
                            sc_uint<32> temp = matrix[i][k];
                            matrix[i][k] = matrix[i][k + 1];
                            matrix[i][k + 1] = temp;
                        }
                    }
                }
            }
            sort_done.write(true);
        }
    }

    // Method to set the done signal
    void set_done() {
        done.write(sort_done.read());
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
