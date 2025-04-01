
#include <systemc.h>

// Module for Matrix Multiplication
SC_MODULE(MatrixMultiplier) {
    sc_in<int> mat1[2][2]; // Input ports for Matrix-1
    sc_in<int> mat2[2][3]; // Input ports for Matrix-2
    sc_out<int> result[2][3]; // Output ports for the result matrix

    // Constructor
    SC_CTOR(MatrixMultiplier) {
        // Process to perform matrix multiplication
        SC_METHOD(multiply);
        sensitive << mat1[0][0] << mat1[0][1] << mat1[1][0] << mat1[1][1]
                  << mat2[0][0] << mat2[0][1] << mat2[0][2]
                  << mat2[1][0] << mat2[1][1] << mat2[1][2];
    }

    // Method to multiply the matrices
    void multiply() {
        int rslt[2][3];
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                rslt[i][j] = 0;
                for (int k = 0; k < 2; k++) {
                    rslt[i][j] += mat1[i][k].read() * mat2[k][j].read();
                }
                result[i][j].write(rslt[i][j]);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> mat1[2][2]; // Signals for Matrix-1
    sc_signal<int> mat2[2][3]; // Signals for Matrix-2
    sc_signal<int> result[2][3]; // Signals for the result matrix

    MatrixMultiplier multiplier_inst; // Instance of the MatrixMultiplier module

    // Constructor
    SC_CTOR(Testbench) : multiplier_inst("multiplier") {
        // Connect signals to ports
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                multiplier_inst.mat1[i][j](mat1[i][j]);
            }
            for (int j = 0; j < 3; j++) {
                multiplier_inst.mat2[i][j](mat2[i][j]);
                multiplier_inst.result[i][j](result[i][j]);
            }
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrices
        mat1[0][0] = 1; mat1[0][1] = 1;
        mat1[1][0] = 2; mat1[1][1] = 2;

        mat2[0][0] = 1; mat2[0][1] = 1; mat2[0][2] = 1;
        mat2[1][0] = 2; mat2[1][1] = 2; mat2[1][2] = 2;

        wait(1, SC_NS); // Wait for the multiplier to process

        // Print the result
        cout << "Multiplication of given two matrices is:\n";
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                cout << result[i][j].read() << "\t";
            }
            cout << endl;
        }

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
