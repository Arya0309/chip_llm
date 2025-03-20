
#include <systemc.h>

// Define matrix dimensions
#define R1 2
#define C1 2
#define R2 2
#define C2 3

// MatrixMultiplier module
SC_MODULE(MatrixMultiplier) {
    // Input ports for matrices
    sc_in<int> mat1[R1][C1];
    sc_in<int> mat2[R2][C2];

    // Output ports for result matrix
    sc_out<int> rslt[R1][C2];

    // Process to multiply matrices
    void multiplyMatrices() {
        int tempRslt[R1][C2];
        
        for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C2; j++) {
                tempRslt[i][j] = 0;
                for (int k = 0; k < R2; k++) {
                    tempRslt[i][j] += mat1[i][k].read() * mat2[k][j].read();
                }
                rslt[i][j].write(tempRslt[i][j]);
            }
        }
    }

    // Constructor to register the process
    SC_CTOR(MatrixMultiplier) {
        SC_METHOD(multiplyMatrices);
        for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C2; j++) {
                sensitive << mat1[i][0]; // Sensitivity list for triggering
                for (int k = 1; k < C1; k++) {
                    sensitive << mat1[i][k];
                }
                for (int k = 0; k < R2; k++) {
                    sensitive << mat2[k][j];
                }
            }
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MatrixMultiplier
    sc_signal<int> mat1Signal[R1][C1];
    sc_signal<int> mat2Signal[R2][C2];
    sc_signal<int> rsltSignal[R1][C2];

    // Instance of MatrixMultiplier
    MatrixMultiplier multiplier;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize matrices
        mat1Signal[0][0] = 1; mat1Signal[0][1] = 1;
        mat1Signal[1][0] = 2; mat1Signal[1][1] = 2;
        
        mat2Signal[0][0] = 1; mat2Signal[0][1] = 1; mat2Signal[0][2] = 1;
        mat2Signal[1][0] = 2; mat2Signal[1][1] = 2; mat2Signal[1][2] = 2;

        // Wait for one delta cycle to let the multiplier process
        wait(1, SC_NS);

        // Print the result matrix
        cout << "Multiplication of given two matrices is:\n";
        for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C2; j++) {
                cout << rsltSignal[i][j].read() << "\t";
            }
            cout << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : multiplier("multiplier") {
        // Connecting signals to the multiplier ports
        for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C1; j++) {
                multiplier.mat1[i][j](mat1Signal[i][j]);
            }
            for (int j = 0; j < C2; j++) {
                multiplier.rslt[i][j](rsltSignal[i][j]);
            }
        }
        for (int i = 0; i < R2; i++) {
            for (int j = 0; j < C2; j++) {
                multiplier.mat2[i][j](mat2Signal[i][j]);
            }
        }

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
