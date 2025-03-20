
#include <systemc.h>

// Module to swap the first and last columns of a 4x4 matrix
SC_MODULE(MatrixColumnSwapper) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> reset;        // Reset signal
    sc_in<sc_matrix<int, 4>> input_matrix;  // Input matrix
    sc_out<sc_matrix<int, 4>> output_matrix; // Output matrix

    // Internal matrix to store the modified matrix
    sc_matrix<int, 4> internal_matrix;

    // Constructor
    SC_CTOR(MatrixColumnSwapper) {
        // Register the process to handle matrix column swapping
        SC_METHOD(swap_columns);
        sensitive << clk.posedge_event() << reset;
    }

    // Method to swap the first and last columns
    void swap_columns() {
        if (reset.read()) {
            // Reset the internal matrix to zero or any initial state
            internal_matrix = sc_matrix<int, 4>(0);
        } else {
            // Copy the input matrix to the internal matrix
            internal_matrix = input_matrix.read();

            // Swap the first and last columns
            for (int i = 0; i < 4; i++) {
                int temp = internal_matrix[i][0];
                internal_matrix[i][0] = internal_matrix[i][3];
                internal_matrix[i][3] = temp;
            }

            // Write the modified matrix to the output
            output_matrix.write(internal_matrix);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;                    // Clock signal
    sc_signal<bool> reset;          // Reset signal
    sc_signal<sc_matrix<int, 4>> input_matrix;  // Input matrix signal
    sc_signal<sc_matrix<int, 4>> output_matrix; // Output matrix signal

    // Instance of the MatrixColumnSwapper module
    MatrixColumnSwapper swapper_inst;

    // Constructor
    SC_CTOR(Testbench)
    : swapper_inst("swapper"), clk("clk", 10, SC_NS, 0.5, 0, SC_NS, true) {
        // Connect signals to ports
        swapper_inst.clk(clk);
        swapper_inst.reset(reset);
        swapper_inst.input_matrix(input_matrix);
        swapper_inst.output_matrix(output_matrix);

        // Process to initialize and run the test
        SC_THREAD(run_test);
    }

    // Thread to run test cases
    void run_test() {
        // Initialize the reset signal
        reset.write(true);
        wait(2, SC_NS); // Hold reset for 2 cycles
        reset.write(false);

        // Initialize the input matrix
        sc_matrix<int, 4> init_matrix = {{8, 9, 7, 6},
                                         {4, 7, 6, 5},
                                         {3, 2, 1, 8},
                                         {9, 9, 7, 7}};
        input_matrix.write(init_matrix);

        // Wait for a few clock cycles to let the swapper process the input
        wait(5, SC_NS);

        // Read and print the output matrix
        sc_matrix<int, 4> result_matrix = output_matrix.read();
        cout << "Modified Matrix:" << endl;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cout << result_matrix[i][j] << " ";
            }
            cout << endl;
        }

        // Stop the simulation
        sc_stop();
    }
};

// Main function to start the simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
