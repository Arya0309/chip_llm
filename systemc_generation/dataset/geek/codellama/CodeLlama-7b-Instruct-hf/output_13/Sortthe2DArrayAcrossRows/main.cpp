scss
#include <systemc.h>

// 2D matrix to store the input data
logic m[4][4];

// Function to perform the matrix sorting
void sortRowWise(int r, int c) {
  // Loop to sort the rows of the matrix
  for (int i = 0; i < r; i++) {
    // Loop to sort the columns of the matrix
    for (int j = 0; j < c; j++) {
      // Loop to perform comparisons and swaps
      for (int k = 0; k < c - j - 1; k++) {
        // Comparison and swap of elements
        if (m[i][k] > m[i][k + 1]) {
          tmp = m[i][k];
          m[i][k] = m[i][k + 1];
          m[i][k + 1] = tmp;
        }
      }
    }
  }
  // Printing the sorted matrix
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      out << "m[" << i << "][" << j << "] = " << m[i][j] << endl;
    }
  }
}

// Incorporating the signals

sc_in<bool> clk;  // Clock signal
sc_in<bool> start;  // Start signal
sc_out<bool> done;  // Done signal

// Module that sorts the matrix row-wise

SC_MODULE(RowWiseSorter) {
  sc_in<int> r;  // Row count
  sc_in<int> c;  // Column count
  sc_in<int> m[4][4];  // Matrix
  sc_out<int> result[4][4];  // Sorted matrix

  // Constructor
  SC_CTOR(RowWiseSorter) {
    // Process to perform sorting
    SC_METHOD(sortRowWise);
    sensitive << r << c << m;
  }

  // Function to perform the matrix sorting
  void sortRowWise() {
    // Calculate the row and column counts
    int rows = r.read();
    int cols = c.read();
     for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        result[i][j] = m[i][j];  // Copy the elements to the sorted matrix
      }
      for (int j = 0; j < cols - 1; j++) {
        for (int k = 0; k < cols - j - 1; k++) {
          if (result[i][k] > result[i][k + 1]) {
            tmp = result[i][k];
            result[i][k] = result[i][k + 1];
            result[i][k + 1] = tmp;
          }
        }
      }
    }
  }
};

// Driver code
void main() {
  int m[4][4];
  done = false;
  while (!done) {
    // Reset the matrix
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        m[i][j] = 0;
      }
    }
    // Initialize the matrix with random values
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        m[i][j] = rand();
      }
    }
    // Sort the matrix
    sortRowWise(4, 4, m);
    // Output the sorted matrix
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cout << "m[" << i << "][" << j << "] = " << m[i][j] << endl;
      }
    }
    done = true;
  }
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
