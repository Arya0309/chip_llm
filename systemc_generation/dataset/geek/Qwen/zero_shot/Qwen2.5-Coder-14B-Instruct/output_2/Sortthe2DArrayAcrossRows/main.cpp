
#include <systemc.h>


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;
    
    // Define the matrix size
    const int ROWS = 4;
    const int COLS = 4;
    
    // Matrix storage
    int matrix[ROWS][COLS];
    
    // Internal state variables
    bool sorting_done;
    int row, col, k;
    
    // Process to handle sorting
    void sort_process() {
        wait(); // Wait for the first clock edge
        
        while (true) {
            // Wait for the start signal
            do {
                wait();
            } while (!start.read());
            
            // Initialize sorting process
            sorting_done = false;
            row = 0;
            col = 0;
            k = 0;
            
            // Sort the matrix row-wise
            while (!sorting_done) {
                // Bubble sort each row
                if (matrix[row][k] > matrix[row][k + 1]) {
                    // Swap elements
                    int temp = matrix[row][k];
                    matrix[row][k] = matrix[row][k + 1];
                    matrix[row][k + 1] = temp;
                }
                
                // Move to the next pair of elements
                k++;
                
                // Check if the end of the row is reached
                if (k == COLS - col - 1) {
                    k = 0;
                    col++;
                    
                    // Check if the end of the current row is reached
                    if (col == COLS) {
                        col = 0;
                        row++;
                        
                        // Check if all rows are sorted
                        if (row == ROWS) {
                            sorting_done = true;
                        }
                    }
                }
                
                wait(); // Wait for the next clock edge
            }
            
            // Set done signal after sorting is complete
            done.write(true);
            wait(); // Wait for the next clock edge
            done.write(false);
        }
    }
    
    SC_CTOR(RowWiseSorter) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
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
