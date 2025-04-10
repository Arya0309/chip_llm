
#include <systemc.h>


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;
    
    // Define the matrix size
    const static int ROWS = 4;
    const static int COLS = 4;
    
    // Matrix to be sorted
    int matrix[ROWS][COLS];
    
    // Internal state variables
    bool started = false;
    int current_row = 0;
    int i, j, k;
    
    // Swap function
    void swap(int &xp, int &yp) {
        int temp = xp;
        xp = yp;
        yp = temp;
    }
    
    // Process to sort the matrix
    SC_CTOR(RowWiseSorter) {
        SC_METHOD(sortMatrix);
        sensitive << clk.pos() << start;
    }
    
    void sortMatrix() {
        if (!started && start.read()) {
            started = true;
            current_row = 0;
        }
        
        if (started) {
            if (current_row < ROWS) {
                // Bubble sort for the current row
                for (i = 0; i < COLS; i++) {
                    for (j = 0; j < COLS - i - 1; j++) {
                        if (matrix[current_row][j] > matrix[current_row][j + 1]) {
                            swap(matrix[current_row][j], matrix[current_row][j + 1]);
                        }
                    }
                }
                current_row++;
            } else {
                started = false;
                done.write(true);
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
