
#include <systemc.h>
#include <iostream>

const int R = 4;
const int C = 4;

// SystemC module to rotate a matrix
SC_MODULE(MatrixRotator) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> rst;          // Reset signal
    sc_in<bool> start;        // Start signal to begin rotation
    sc_out<bool> done;        // Done signal indicating rotation completion
    sc_out<int> rotated_mat[R][C]; // Output ports for the rotated matrix

    int mat[R][C];           // Internal storage for the matrix
    bool rotating;           // Flag to indicate if rotation is in progress

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Register the rotation process
        SC_METHOD(rotate_matrix);
        sensitive << clk.posedge_event() << rst.posedge_event();
        dont_initialize();
        rotating = false;
        done.write(false);
    }

    // Method to rotate the matrix
    void rotate_matrix() {
        if (rst.read()) {
            rotating = false;
            done.write(false);
            // Clear the output ports
            for (int i = 0; i < R; i++) {
                for (int j = 0; j < C; j++) {
                    rotated_mat[i][j].write(0);
                }
            }
        } else if (start.read() && !rotating) {
            rotating = true;
            done.write(false);
            // Copy input matrix to internal storage
            for (int i = 0; i < R; i++) {
                for (int j = 0; j < C; j++) {
                    mat[i][j] = rotated_mat[i][j].read();
                }
            }
            // Perform the rotation
            int row = 0, col = 0;
            int prev, curr;
            while (row < R && col < C) {
                if (row + 1 == R || col + 1 == C) break;

                prev = mat[row + 1][col];
                for (int i = col; i < C; i++) {
                    curr = mat[row][i];
                    mat[row][i] = prev;
                    prev = curr;
                }
                row++;

                for (int i = row; i < R; i++) {
                    curr = mat[i][C - 1];
                    mat[i][C - 1] = prev;
                    prev = curr;
                }
                C--;

                if (row < R) {
                    for (int i = C - 1; i >= col; i--) {
                        curr = mat[R - 1][i];
                        mat[R - 1][i] = prev;
                        prev = curr;
                    }
                }
                R--;

                if (col < C) {
                    for (int i = R - 1; i >= row; i--) {
                        curr = mat[i][col];
                        mat[i][col] = prev;
                        prev = curr;
                    }
                }
                col++;
            }

            // Write the rotated matrix to output ports
            for (int i = 0; i < R; i++) {
                for (int j = 0; j < C; j++) {
                    rotated_mat[i][j].write(mat[i][j]);
                }
            }
            rotating = false;
            done.write(true);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;             // Clock signal
    sc_signal<bool> rst;      // Reset signal
    sc_signal<bool> start;    // Start signal
    sc_signal<bool> done;     // Done signal
    sc_signal<int> rotated_mat[R][C]; // Signals for the rotated matrix

    MatrixRotator rotator_inst; // Instance of the MatrixRotator module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS, 0.5), rotator_inst("rotator") {
        // Connect signals to ports
        rotator_inst.clk(clk);
        rotator_inst.rst(rst);
        rotator_inst.start(start);
        rotator_inst.done(done);
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                rotator_inst.rotated_mat[i][j](rotated_mat[i][j]);
            }
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrix values
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                rotated_mat[i][j].write(i * C + j + 1);
            }
        }

        // Reset the system
        rst.write(true);
        wait(1, SC_NS);
        rst.write(false);

        // Start the rotation
        start.write(true);
        wait(1, SC_NS);
        start.write(false);

        // Wait for the rotation to complete
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Print the rotated matrix
        std::cout << "Rotated Matrix:" << std::endl;
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                std::cout << rotated_mat[i][j].read() << " ";
            }
            std::cout << std::endl;
        }

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
