
#include <systemc.h>


SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;      // Clock signal
    sc_in<bool> start;    // Start signal
    sc_out<bool> done;   // Done signal

    int m[4][4] = {{9, 8, 7, 1}, {7, 3, 0, 2}, {9, 5, 3, 2}, {6, 3, 1, 2}};
    int r = 4;
    int c = 4;
    bool sorting_done = false;
    bool start_received = false;
    sc_event sort_done_event;  // Event to signal sorting completion

    SC_CTOR(RowWiseSorter) {
        SC_THREAD(sort);
        sensitive << clk.pos();
        dont_initialize();
    }

    void sort() {
        while (true) {
            wait(clk.pos()); // Wait for the clock edge

            if (start.read()) {
                start_received = true;
            }

            if (start_received && !sorting_done) {
                // Perform sorting
                for (int i = 0; i < r; i++) {
                    for (int j = 0; j < c; j++) {
                        for (int k = 0; k < c - j - 1; k++) {
                            if (m[i][k] > m[i][k + 1]) {
                                swap(m[i][k], m[i][k + 1]);
                            }
                        }
                    }
                }

                sorting_done = true;
                sort_done_event.notify();  // Notify that sorting is done
            }

            if (sorting_done) {
                wait(sort_done_event);  // Wait for the sorting to be done
                done.write(true);
                wait(clk.pos());  // Wait for the next clock cycle before setting done to false
                done.write(false);
                sorting_done = false;
                start_received = false;
            }
        }
    }

    void swap(int &xp, int &yp) {
        int temp = xp;
        xp = yp;
        yp = temp;
    }

    // Add load_matrix and read_matrix methods
    void load_matrix(int matrix[4][4]) {
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                m[i][j] = matrix[i][j];
            }
        }
    }

    void read_matrix(int matrix[4][4]) {
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                matrix[i][j] = m[i][j];
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
