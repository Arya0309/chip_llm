scss
#include <systemc.h>

// Define the module

SC_MODULE(RowWiseSorter) {
    sc_in<int> m[MAX_ROWS][MAX_COLS];  // Input port for the matrix
    sc_in<int> r;  // Input port for the number of rows
    sc_in<int> c;  // Input port for the number of columns
    sc_out<int> sorted_matrix[MAX_ROWS][MAX_COLS];  // Output port for the sorted matrix

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Process to perform sorting
        SC_METHOD(sort);
        sensitive << m << r << c;
    }

    // Method to sort the rows of the matrix
    void sort() {
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c - 1; j++) {
                if (m[i][j] > m[i][j + 1]) {
                    // Swap the elements
                    int temp = m[i][j];
                    m[i][j] = m[i][j + 1];
                    m[i][j + 1] = temp;
                }
            }
        }
        // Print the sorted matrix
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                sorted_matrix[i][j] = m[i][j];
            }
        }
    }
};

// Top-level module

SC_MODULE(Top) {
    // Input and output signals
    sc_in_clk clk;
    sc_in<bool> start;
    sc_out<bool> done;

    // Define the adder module
    RowWiseSorter sort_module;

    // Constructor
    SC_CTOR(Top) {
        sort_module(*this);
    }

    // Function to initialize the variables
    void initialize() {
        int m[MAX_ROWS][MAX_COLS] = {
            {9, 8, 7, 1},
            {7, 3, 0, 2},
            {9, 5, 3, 2},
            {6, 3, 1, 2}
        };
        int r = sizeof(m) / sizeof(m[0]);
        int c = sizeof(m[0]) / sizeof(m[0][0]);
        sort_module.m = m;
        sort_module.r = r;
        sort_module.c = c;
    }

    // Function to start the simulation
    void start_simulation() {
        initialize();
        sc_start();
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
