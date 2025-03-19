
#include <systemc.h>

// Define a module to sort a 2D matrix row-wise
SC_MODULE(MatrixSorter) {
    // Ports for matrix input and output
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> rst;  // Reset signal
    sc_out<sc_matrix<int>> sortedMatrix;  // Output port for sorted matrix

    // Internal variables
    int matrix[4][4];  // Matrix to be sorted
    bool done;  // Flag to indicate sorting completion

    // Constructor to initialize the process
    SC_CTOR(MatrixSorter) {
        // Register the process
        SC_METHOD(sortMatrix);
        sensitive << clk.posedge_event() << rst;
        dont_initialize();

        // Initialize the matrix
        matrix[0][0] = 9; matrix[0][1] = 8; matrix[0][2] = 7; matrix[0][3] = 1;
        matrix[1][0] = 7; matrix[1][1] = 3; matrix[1][2] = 0; matrix[1][3] = 2;
        matrix[2][0] = 9; matrix[2][1] = 5; matrix[2][2] = 3; matrix[2][3] = 2;
        matrix[3][0] = 6; matrix[3][1] = 3; matrix[3][2] = 1; matrix[3][3] = 2;
        done = false;
    }

    // Process to sort the matrix
    void sortMatrix() {
        if (rst) {
            done = false;
        } else if (!done) {
            int r = 4;  // Number of rows
            int c = 4;  // Number of columns

            // Bubble sort for each row
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    for (int k = 0; k < c - j - 1; k++) {
                        if (matrix[i][k] > matrix[i][k + 1]) {
                            std::swap(matrix[i][k], matrix[i][k + 1]);
                        }
                    }
                }
            }

            // Mark sorting as done
            done = true;
        }

        // Output the sorted matrix
        if (done) {
            sc_matrix<int> outputMatrix(r, c);
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    outputMatrix[i][j] = matrix[i][j];
                }
            }
            sortedMatrix.write(outputMatrix);
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MatrixSorter
    sc_signal<bool> clk;
    sc_signal<bool> rst;
    sc_signal<sc_matrix<int>> sortedMatrix;

    // Instance of MatrixSorter
    MatrixSorter sorter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize reset signal
        rst.write(true);
        wait(1, SC_NS);
        rst.write(false);

        // Wait for sorting to complete
        wait(10, SC_NS);

        // Print the sorted matrix
        sc_matrix<int> matrix = sortedMatrix.read();
        int r = 4;
        int c = 4;
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : sorter("sorter") {
        // Connecting signals to the sorter ports
        sorter.clk(clk);
        sorter.rst(rst);
        sorter.sortedMatrix(sortedMatrix);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);

        // Generate clock signal
        SC_THREAD(generateClock);
    }

    // Process to generate clock signal
    void generateClock() {
        while (true) {
            clk.write(false);
            wait(5, SC_NS);
            clk.write(true);
            wait(5, SC_NS);
        }
    }
};

// Main function to start simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
