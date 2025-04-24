scss
#include <systemc.h>

// Module that sorts a 2D matrix row-wise

SC_MODULE(RowWiseSorter) {
    sc_in<int> m[4][4];  // Input port for the 2D matrix
    sc_in<int> r;  // Input port for the number of rows in the matrix
    sc_in<int> c;  // Input port for the number of columns in the matrix
    sc_out<int> sorted_matrix[4][4];  // Output port for the sorted matrix

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Process to sort matrix
        SC_METHOD(sort_matrix);
        sensitive << clk;
    }

    // Method to sort the matrix
    void sort_matrix() {
        int temp[4];  // Temporary array to store a row of the matrix
        bool swap;  // Flag to indicate if a swap has been made

        // Loop through each row of the matrix
        for (int i = 0; i < r; i++) {
            // Reset the flag
            swap = false;

            // Loop through each element in the row
            for (int j = 0; j < c; j++) {
                // Compare each element to its right neighbor
                if (m[i][j] > m[i][j + 1]) {
                    // Swap the two elements
                    temp[j] = m[i][j];
                    m[i][j] = m[i][j + 1];
                    m[i][j + 1] = temp[j];

                    // Set the flag to indicate a swap has been made
                    swap = true;
                }
            }

            // If a swap has been made, repeat the process for the next row
            if (swap)
                i--;
        }
    }

    // Method to print the sorted matrix
    void print_matrix() {
        for (int i = 0; i < r; i++) {
            cout << "Row " << i << ": ";
            for (int j = 0; j < c; j++)
                cout << m[i][j] << " ";
            cout << endl;
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
