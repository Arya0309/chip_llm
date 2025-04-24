c
// Include the SystemC headers
#include <systemc.h>

// Define the struct to hold the matrix elements
struct Matrix {
    int rows; // Number of rows in the matrix
    int cols; // Number of columns in the matrix
    int* data; // Pointer to the matrix data
};

// Define the module to sort the rows of the matrix

SC_MODULE(RowWiseSorter) {
    // Input ports
    sc_in<int> rows; // Number of rows in the matrix
    sc_in<int> cols; // Number of columns in the matrix
    sc_in<int> data; // Matrix data to be sorted

    // Output ports
    sc_out<int> sorted; // The sorted matrix

    // Constructor
    SC_CTOR(RowWiseSorter) {
        // Process to sort the rows of the matrix
        SC_METHOD(sort_rows);
        sensitive << rows << cols << data;
    }

    // Method to sort the rows of the matrix
    void sort_rows() {
        // Create a temporary array to hold the sorted rows
        int* sorted_rows = new int[rows];

        // Loop over the rows of the matrix
        for (int i = 0; i < rows; i++) {
            // Copy the values of the current row to the temporary array
            for (int j = 0; j < cols; j++) {
                sorted_rows[j] = data[i][j];
            }

            // Sort the numbers in the current row
            bubble_sort(sorted_rows, cols);

            // Write the sorted row back to the matrix
            for (int j = 0; j < cols; j++) {
                data[i][j] = sorted_rows[j];
            }
        }

        // Free the temporary array
        delete[] sorted_rows;
    }

    // Method to implement the bubble sort algorithm
    void bubble_sort(int* arr, int n) {
        // Loop over the array and swap elements if necessary
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    swap(arr[j], arr[j + 1]);
                }
            }
        }
    }

    // Method to swap two integers
    void swap(int* a, int* b) {
        int temp = *a;
        *a = *b;
        *b = temp;
    }
};

// Driver code to test the module
int main() {
    // Create a new matrix to sort
    Matrix m = {{9, 8, 7, 1},
                  {7, 3, 0, 2},
                  {9, 5, 3, 2},
                  {6, 3, 1, 2}};

    // Create the RowWiseSorter module
    RowWiseSorter sr(m.rows, m.cols, m.data);

    // Run the sorting process
    sr.run();

    // Print the sorted matrix
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            cout << m.data[i][j] << " ";
        }
        cout << endl;
    }

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
