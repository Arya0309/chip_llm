
#include <systemc.h>


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> start;        // Start signal
    sc_out<bool> done;         // Done signal
    sc_inout<int> matrix[4][4]; // Input/Output matrix

    // Constructor
    SC_CTOR(RowWiseSorter) {
        SC_THREAD(sortRows);
        sensitive << clk.pos();
        dont_initialize();

        SC_THREAD(checkDone);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                sensitive << matrix[i][j];
            }
        }
    }

    // Event to trigger sorting completion
    sc_event sort_done_event;

    // Method to sort each row of the matrix
    void sortRows() {
        while (true) {
            wait(start); // Wait for start signal

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    for (int k = 0; k < 4 - j - 1; k++) {
                        if (matrix[i][k].read() > matrix[i][k + 1].read()) {
                            int temp = matrix[i][k].read();
                            matrix[i][k].write(matrix[i][k + 1].read());
                            matrix[i][k + 1].write(temp);
                        }
                    }
                }
            }

            sort_done_event.notify(); // Notify that sorting is done
        }
    }

    // Method to check if sorting is done
    void checkDone() {
        while (true) {
            wait(sort_done_event); // Wait for sorting completion

            bool isSorted = true;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 3; j++) {
                    if (matrix[i][j].read() > matrix[i][j + 1].read()) {
                        isSorted = false;
                        break;
                    }
                }
                if (!isSorted) break;
            }

            done.write(isSorted); // Set done signal based on sorting status
        }
    }

    // Method to load the input matrix
    void load_matrix(const int input_matrix[4][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                matrix[i][j].write(input_matrix[i][j]);
            }
        }
    }

    // Method to read the sorted matrix
    void read_matrix(int output_matrix[4][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                output_matrix[i][j] = matrix[i][j].read();
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
