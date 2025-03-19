
#include <systemc.h>

// Module to swap the first and last columns of a matrix
SC_MODULE(MatrixSwapper) {
    // Signal for matrix input and output
    sc_in<sc_matrix<int>> in_matrix;
    sc_out<sc_matrix<int>> out_matrix;

    // Internal storage for matrix
    sc_matrix<int> matrix;

    // Process to swap the first and last columns
    void swapColumns() {
        matrix = in_matrix.read();
        for (int i = 0; i < 4; i++) {
            int temp = matrix[i][0];
            matrix[i][0] = matrix[i][3];
            matrix[i][3] = temp;
        }
        out_matrix.write(matrix);
    }

    // Constructor to register the process
    SC_CTOR(MatrixSwapper) {
        SC_METHOD(swapColumns);
        sensitive << in_matrix;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MatrixSwapper
    sc_signal<sc_matrix<int>> in_matrix;
    sc_signal<sc_matrix<int>> out_matrix;

    // Instance of MatrixSwapper
    MatrixSwapper swapper;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the matrix
        sc_matrix<int> input_matrix(4, 4);
        input_matrix[0] = {8, 9, 7, 6};
        input_matrix[1] = {4, 7, 6, 5};
        input_matrix[2] = {3, 2, 1, 8};
        input_matrix[3] = {9, 9, 7, 7};

        // Drive the input matrix
        in_matrix.write(input_matrix);

        // Wait for one delta cycle to let the swapper process
        wait(1, SC_NS);

        // Read the output matrix
        sc_matrix<int> output_matrix = out_matrix.read();

        // Print the results
        cout << "Matrix after swapping first and last columns:" << endl;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cout << output_matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : swapper("swapper") {
        // Connecting signals to the swapper ports
        swapper.in_matrix(in_matrix);
        swapper.out_matrix(out_matrix);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

// Custom data type for matrix
typedef sc_fixed<32, 16, SC_RND, SC_SAT> sc_matrix_elem_t;
typedef sc_matrix<sc_matrix_elem_t> sc_matrix_t;

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
