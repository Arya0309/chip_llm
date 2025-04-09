
   #include <systemc.h>
   #include <iostream>

   
SC_MODULE(RowWiseSorter) {
       sc_in<bool> clk;
       sc_in<bool> start;
       sc_out<bool> done;
       sc_vector<sc_vector<sc_signal<int>>> matrix;
       int rows, cols;

       SC_CTOR(RowWiseSorter) : matrix("matrix", 4) {
           for (int i = 0; i < 4; ++i) {
               matrix[i] = sc_vector<sc_signal<int>>("row", 4);
           }
           rows = 4;
           cols = 4;
           SC_THREAD(sortRowWise);
           sensitive << clk.pos();
           dont_initialize();
       }

       void sortRowWise() {
           while (true) {
               wait(start.event());
               for (int i = 0; i < rows; i++) {
                   for (int j = 0; j < cols; j++) {
                       for (int k = 0; k < cols - j - 1; k++) {
                           if (matrix[i][k].read() > matrix[i][k + 1].read()) {
                               int temp = matrix[i][k].read();
                               matrix[i][k].write(matrix[i][k + 1].read());
                               matrix[i][k + 1].write(temp);
                           }
                       }
                   }
               }
               done.write(true);
               wait();
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
