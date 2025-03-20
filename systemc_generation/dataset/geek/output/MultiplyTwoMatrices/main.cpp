
#include <systemc.h>

// Module to perform matrix multiplication
SC_MODULE(MatrixMultiplier) {
    sc_in<bool> clk;                      // Clock signal
    sc_in<bool> rst;                      // Reset signal
    sc_in<sc_uint<32>> mat1[2][2];        // Input matrix 1 (2x2)
    sc_in<sc_uint<32>> mat2[2][3];        // Input matrix 2 (2x3)
    sc_out<sc_uint<32>> result[2][3];     // Output matrix (2x3)

    // Internal state variables
    int rslt[2][3];
    int i, j, k;
    bool done;

    // Constructor
    SC_CTOR(MatrixMultiplier) {
        // Sensitivity list for the process
        SC_METHOD(matrix_multiply);
        sensitive << clk.posedge_event() << rst.posedge_event();
    }

    // Method to perform matrix multiplication
    void matrix_multiply() {
        if (rst) {
            // Reset the state
            i = 0;
            j = 0;
            k = 0;
            done = false;
            for (int x = 0; x < 2; x++) {
                for (int y = 0; y < 3; y++) {
                    rslt[x][y] = 0;
                    result[x][y].write(0);
                }
            }
        } else {
            if (!done) {
                if (k < 2) {
                    rslt[i][j] += mat1[i][k].read() * mat2[k][j].read();
                    k++;
                } else {
                    result[i][j].write(rslt[i][j]);
                    j++;
                    if (j >= 3) {
                        j = 0;
                        i++;
                        if (i >= 2) {
                            i = 0;
                            done = true;
                        }
                    }
                    k = 0;
                    rslt[i][j] = 0;
                }
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk;                  // Clock signal
    sc_signal<bool> rst;                  // Reset signal
    sc_signal<sc_uint<32>> mat1[2][2];    // Input matrix 1 (2x2)
    sc_signal<sc_uint<32>> mat2[2][3];    // Input matrix 2 (2x3)
    sc_signal<sc_uint<32>> result[2][3];   // Output matrix (2x3)

    MatrixMultiplier multiplier_inst;     // Instance of the MatrixMultiplier module

    // Constructor
    SC_CTOR(Testbench) : multiplier_inst("multiplier") {
        // Connect signals to ports
        multiplier_inst.clk(clk);
        multiplier_inst.rst(rst);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                multiplier_inst.mat1[i][j](mat1[i][j]);
            }
        }
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                multiplier_inst.mat2[i][j](mat2[i][j]);
                multiplier_inst.result[i][j](result[i][j]);
            }
        }

        // Process to generate clock
        SC_THREAD(gen_clk);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to generate clock signal
    void gen_clk() {
        rst.write(true);
        wait(5, SC_NS);
        rst.write(false);
        while (true) {
            clk.write(false);
            wait(1, SC_NS);
            clk.write(true);
            wait(1, SC_NS);
        }
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrices
        mat1[0][0].write(1);
        mat1[0][1].write(1);
        mat1[1][0].write(2);
        mat1[1][1].write(2);

        mat2[0][0].write(1);
        mat2[0][1].write(1);
        mat2[0][2].write(1);
        mat2[1][0].write(2);
        mat2[1][1].write(2);
        mat2[1][2].write(2);

        // Wait for the multiplication to complete
        wait(20, SC_NS);

        // Print results
        cout << "Multiplication of given two matrices is:" << endl;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                cout << result[i][j].read() << "\t";
            }
            cout << endl;
        }

        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
