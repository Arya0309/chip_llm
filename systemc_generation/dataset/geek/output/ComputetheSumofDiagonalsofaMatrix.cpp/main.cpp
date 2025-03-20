
#include <systemc.h>

// Define a SystemC module for calculating diagonal sums
SC_MODULE(DiagonalSumsCalculator) {
    // Input port for the matrix size
    sc_in<int> matrixSize;
    
    // Input port for the matrix elements
    sc_in<int> matrix[MAX][MAX];
    
    // Output ports for the diagonal sums
    sc_out<int> principalDiagonalSum;
    sc_out<int> secondaryDiagonalSum;

    // Method to calculate the diagonal sums
    void calculateDiagonalSums() {
        int principal = 0, secondary = 0;
        int n = matrixSize.read();
        
        // Traverse the matrix to calculate the sums
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j) {
                    principal += matrix[i][j].read();
                }
                if ((i + j) == (n - 1)) {
                    secondary += matrix[i][j].read();
                }
            }
        }
        
        // Write the results to the output ports
        principalDiagonalSum.write(principal);
        secondaryDiagonalSum.write(secondary);
    }

    // Constructor to register the method
    SC_CTOR(DiagonalSumsCalculator) {
        SC_METHOD(calculateDiagonalSums);
        sensitive << matrixSize;
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                sensitive << matrix[i][j];
            }
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with DiagonalSumsCalculator
    sc_signal<int> matrixSize;
    sc_signal<int> matrix[MAX][MAX];
    sc_signal<int> principalDiagonalSum, secondaryDiagonalSum;

    // Instance of DiagonalSumsCalculator
    DiagonalSumsCalculator calculator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize matrix size
        matrixSize = 4;

        // Initialize matrix elements
        matrix[0][0] = 1; matrix[0][1] = 2; matrix[0][2] = 3; matrix[0][3] = 4;
        matrix[1][0] = 5; matrix[1][1] = 6; matrix[1][2] = 7; matrix[1][3] = 8;
        matrix[2][0] = 1; matrix[2][1] = 2; matrix[2][2] = 3; matrix[2][3] = 4;
        matrix[3][0] = 5; matrix[3][1] = 6; matrix[3][2] = 7; matrix[3][3] = 8;

        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);

        // Print the results
        cout << "Principal Diagonal: " << principalDiagonalSum.read() << endl;
        cout << "Secondary Diagonal: " << secondaryDiagonalSum.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : calculator("calculator") {
        // Connecting signals to the calculator ports
        calculator.matrixSize(matrixSize);
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                calculator.matrix[i][j](matrix[i][j]);
            }
        }
        calculator.principalDiagonalSum(principalDiagonalSum);
        calculator.secondaryDiagonalSum(secondaryDiagonalSum);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
