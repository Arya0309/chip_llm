
#include <systemc.h>


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal
    sc_out<bool> done;     // Done signal

    // 2D matrix to store input data
    int m[4][4];
    int r = 4;  // Number of rows
    int c = 4;  // Number of columns

    // Flag to indicate if sorting has started
    bool sorting_started = false;

    // Process to handle clock edges
    SC_METHOD(process_clk);
    sensitive << clk.pos();

    // Method to perform row-wise sorting
    void sortRowWise() {
        // Sorting each row
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                for (int k = 0; k < c - j - 1; k++) {
                    if (m[i][k] > m[i][k + 1]) {
                        swap(m[i][k], m[i][k + 1]);
                    }
                }
            }
        }
    }

    // Process to handle sorting logic
    void process_clk() {
        if (start.read()) {
            sorting_started = true;
        }

        if (sorting_started && !done.read()) {
            sortRowWise();
            done.write(true);  // Signal that sorting is complete
        }
    }

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Initialize matrix data
        m[0][0] = 9; m[0][1] = 8; m[0][2] = 7; m[0][3] = 1;
        m[1][0] = 7; m[1][1] = 3; m[1][2] = 0; m[1][3] = 2;
        m[2][0] = 9; m[2][1] = 5; m[2][2] = 3; m[2][3] = 2;
        m[3][0] = 6; m[3][1] = 3; m[3][2] = 1; m[3][3] = 2;
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
