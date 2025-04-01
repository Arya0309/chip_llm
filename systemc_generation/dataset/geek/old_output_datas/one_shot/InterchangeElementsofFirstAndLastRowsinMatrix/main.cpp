
#include <systemc.h>

// Module to interchange first and last columns of a matrix
SC_MODULE(MatrixInterchanger) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> rst;  // Reset signal
    sc_out<bool> done; // Done signal indicating completion of operation

    sc_in<int> mat_in[4][4]; // Input matrix
    sc_out<int> mat_out[4][4]; // Output matrix

    // Internal matrix to hold the data
    int matrix[4][4];

    // Constructor
    SC_CTOR(MatrixInterchanger) {
        // Sensitivity list for the main process
        SC_METHOD(interchange);
        sensitive << clk.posedge_event();
        dont_initialize();

        // Reset process
        SC_METHOD(reset);
        sensitive << rst;
    }

    // Reset method to initialize the matrix
    void reset() {
        if (rst.read()) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    matrix[i][j] = mat_in[i][j].read();
                }
            }
            done.write(false);
        }
    }

    // Main process to interchange first and last columns
    void interchange() {
        if (!rst.read()) {
            for (int i = 0; i < 4; ++i) {
                int temp = matrix[i][0];
                matrix[i][0] = matrix[i][3];
                matrix[i][3] = temp;
            }
            done.write(true);
            // Write the modified matrix to output
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    mat_out[i][j].write(matrix[i][j]);
                }
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk;  // Clock signal
    sc_signal<bool> rst;  // Reset signal
    sc_signal<bool> done; // Done signal

    sc_signal<int> mat_in[4][4]; // Input matrix
    sc_signal<int> mat_out[4][4]; // Output matrix

    MatrixInterchanger matrix_interchanger_inst; // Instance of the MatrixInterchanger module

    // Constructor
    SC_CTOR(Testbench) : matrix_interchanger_inst("matrix_interchanger") {
        // Connect signals to ports
        matrix_interchanger_inst.clk(clk);
        matrix_interchanger_inst.rst(rst);
        matrix_interchanger_inst.done(done);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                matrix_interchanger_inst.mat_in[i][j](mat_in[i][j]);
                matrix_interchanger_inst.mat_out[i][j](mat_out[i][j]);
            }
        }

        // Process to generate clock signal
        SC_THREAD(generate_clock);
        // Process to run tests
        SC_THREAD(run_tests);
        // Process to monitor the done signal
        SC_THREAD(check_done);
    }

    // Thread to generate clock signal
    void generate_clock() {
        clk = 0;
        while (true) {
            wait(5, SC_NS); // Half period
            clk = !clk;
        }
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrix
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                mat_in[i][j] = ((int(*)[4]){{8, 9, 7, 6},
                                            {4, 7, 6, 5},
                                            {3, 2, 1, 8},
                                            {9, 9, 7, 7}})[i][j];
            }
        }
        rst = 1;
        wait(10, SC_NS);
        rst = 0;
        wait(done.posedge_event()); // Wait until the operation is done

        // Print the modified matrix
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                cout << mat_out[i][j].read() << " ";
            }
            cout << endl;
        }

        sc_stop(); // Stop the simulation
    }

    // Thread to monitor the done signal
    void check_done() {
        while (true) {
            if (done.read()) {
                cout << "Matrix interchanging completed." << endl;
            }
            wait(1, SC_NS);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
