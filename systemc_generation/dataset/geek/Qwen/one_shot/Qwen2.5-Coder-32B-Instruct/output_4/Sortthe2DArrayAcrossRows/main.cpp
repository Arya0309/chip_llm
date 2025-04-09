
#include <systemc.h>

// Define a module for sorting a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal to initiate sorting
    sc_out<bool> done;    // Done signal indicating sorting completion

    // Internal signals and variables
    static const int ROWS = 4;
    static const int COLS = 4;
    int matrix[ROWS][COLS];
    bool isSorting;
    int currentRow;
    int currentCol;
    int innerLoopCounter;

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Initialize matrix with predefined values
        matrix[0][0] = 9; matrix[0][1] = 8; matrix[0][2] = 7; matrix[0][3] = 1;
        matrix[1][0] = 7; matrix[1][1] = 3; matrix[1][2] = 0; matrix[1][3] = 2;
        matrix[2][0] = 9; matrix[2][1] = 5; matrix[2][2] = 3; matrix[2][3] = 2;
        matrix[3][0] = 6; matrix[3][1] = 3; matrix[3][2] = 1; matrix[3][3] = 2;

        // Reset sorting flags and counters
        isSorting = false;
        currentRow = 0;
        currentCol = 0;
        innerLoopCounter = 0;

        // Process to handle sorting
        SC_METHOD(sortProcess);
        sensitive << clk.pos();
    }

    // Sorting process method
    void sortProcess() {
        if (start.read()) {
            if (!isSorting) {
                isSorting = true;
                currentRow = 0;
                currentCol = 0;
                innerLoopCounter = 0;
                done.write(false);
            }
        }

        if (isSorting) {
            // Perform sorting using bubble sort logic
            if (innerLoopCounter < COLS - currentCol - 1) {
                if (matrix[currentRow][innerLoopCounter] > matrix[currentRow][innerLoopCounter + 1]) {
                    // Swap elements
                    int temp = matrix[currentRow][innerLoopCounter];
                    matrix[currentRow][innerLoopCounter] = matrix[currentRow][innerLoopCounter + 1];
                    matrix[currentRow][innerLoopCounter + 1] = temp;
                }
                innerLoopCounter++;
            } else {
                innerLoopCounter = 0;
                currentCol++;
                if (currentCol >= COLS) {
                    currentCol = 0;
                    currentRow++;
                    if (currentRow >= ROWS) {
                        // Sorting completed
                        isSorting = false;
                        done.write(true);
                        // Print the sorted matrix
                        for (int i = 0; i < ROWS; i++) {
                            for (int j = 0; j < COLS; j++) {
                                cout << matrix[i][j] << " ";
                            }
                            cout << endl;
                        }
                    }
                }
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
