
#include <systemc.h>

// Module to interchange the first and last rows of a matrix
SC_MODULE(MatrixInterchanger) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> rst;  // Reset signal
    sc_in<sc_matrix<int, 4, 4>> input_matrix;  // Input matrix
    sc_out<sc_matrix<int, 4, 4>> output_matrix; // Output matrix

    // Internal storage for the matrix
    sc_matrix<int, 4, 4> matrix;

    // Constructor
    SC_CTOR(MatrixInterchanger) {
        // Register the method to process the matrix
        SC_METHOD(interchangeRows);
        sensitive << clk.posedge_event() << rst.posedge_event();
    }

    // Method to interchange the first and last rows
    void interchangeRows() {
        if (rst.read()) {
            // Reset matrix to initial state if reset is active
            matrix = input_matrix.read();
        } else {
            // Swap the first and last rows
            for (int i = 0; i < 4; ++i) {
                int temp = matrix[0][i];
                matrix[0][i] = matrix[3][i];
                matrix[3][i] = temp;
            }
            // Write the modified matrix to the output
            output_matrix.write(matrix);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk; // Clock signal
    sc_signal<bool> rst; // Reset signal
    sc_signal<sc_matrix<int, 4, 4>> input_matrix; // Input matrix signal
    sc_signal<sc_matrix<int, 4, 4>> output_matrix; // Output matrix signal

    MatrixInterchanger matrix_interchanger_inst; // Instance of the MatrixInterchanger module

    // Constructor
    SC_CTOR(Testbench) : matrix_interchanger_inst("matrix_interchanger"), clk("clk", 10, SC_NS) {
        // Connect signals to ports
        matrix_interchanger_inst.clk(clk);
        matrix_interchanger_inst.rst(rst);
        matrix_interchanger_inst.input_matrix(input_matrix);
        matrix_interchanger_inst.output_matrix(output_matrix);

        // Process to initialize and run the test
        SC_THREAD(run_test);
    }

    // Thread to run test cases
    void run_test() {
        // Initialize the input matrix
        sc_matrix<int, 4, 4> init_matrix = {{8, 9, 7, 6},
                                            {4, 7, 6, 5},
                                            {3, 2, 1, 8},
                                            {9, 9, 7, 7}};
        input_matrix.write(init_matrix);
        rst.write(true); // Assert reset
        wait(1, SC_NS);
        rst.write(false); // Deassert reset
        wait(1, SC_NS);

        // Print the output matrix
        sc_matrix<int, 4, 4> result_matrix = output_matrix.read();
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                cout << result_matrix[i][j] << " ";
            }
            cout << endl;
        }

        // Stop the simulation
        sc_stop();
    }
};

// Custom matrix type definition
template<typename T, int ROWS, int COLS>
class sc_matrix : public sc_vector<sc_vector<T>> {
public:
    sc_matrix(sc_module_name name) : sc_vector<sc_vector<T>>(name, ROWS, sc_gen_unique_name("row")) {
        for (int i = 0; i < ROWS; ++i) {
            (*this)[i].init(COLS, sc_gen_unique_name("col"));
        }
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
