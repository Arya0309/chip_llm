scss
#include <systemc.h>

// Module for sorting a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    // Declare the signals that the module will use
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Declare the ports for the matrix
    sc_in<int> matrix[4];
    sc_out<int> sorted_matrix[4];

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Set up the sensitivity list for the sort method
        SC_METHOD(sort);
        sensitive << clk.pos();
        reset_signals_ascending();

        // Set up the sensitivity list for the print method
        SC_METHOD(print);
        sensitive << clk.pos();
    }

    // Method for sorting the matrix
    void sort() {
        // Sort the matrix using the "std::sort" algorithm
        for (int i = 0; i < r; i++) {
            // Iterate over each row
            for (int j = 0; j < c; j++) {
                // Iterate over each element in the row
                for (int k = 0; k < c - j - 1; k++) {
                    // Iterate over the rest of the elements in the row
                    if (matrix[i][k] > matrix[i][k + 1]) {
                        // Swap the elements if necessary
                        swap(matrix[i][k], matrix[i][k + 1]);
                    }
                }
            }
        }
    }

    // Method for printing the sorted matrix
    void print() {
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                // Print the sorted matrix
                printf("%d ", sorted_matrix[i][j]);
            }
            printf("\n");
        }
    }
};

// Driver code
int main() {
    RowWiseSorter sort_matrix();
    int matrix[4] = {{9, 8, 7, 1},
                    {7, 3, 0, 2},
                    {9, 5, 3, 2},
                    {6, 3, 1, 2}};
    int r = 4;
    int c = 4;
    int n = r * c;
    sort_matrix.start = tb.MakeInput();
    sort_matrix.clk = tb.MakeInput();
    sort_matrix.done = tb.MakeOutput();
    sort_matrix.matrix = tb.MakeInput();
    sort_matrix.sorted_matrix = tb.MakeOutput();
    tb.DriveInput(sort_matrix.start, 0);
    tb.DriveInput(sort_matrix.clk, 0);
    tb.DriveInput(sort_matrix.matrix, matrix, n * sizeof(int));
    tb.WatchOutput(sort_matrix.sorted_matrix, n * sizeof(int));
    tb.Run();
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
