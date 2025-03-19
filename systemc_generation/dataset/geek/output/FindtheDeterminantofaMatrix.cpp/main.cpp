
#include <systemc.h>

// Module to calculate the determinant of a matrix
SC_MODULE(DeterminantCalculator) {
    // Input port for matrix size
    sc_in<int> n;
    
    // Input ports for matrix elements
    sc_in<double> matrix[105][105];
    
    // Output port for determinant
    sc_out<double> determinant;
    
    // Internal storage for matrix
    double a[105][105];
    
    // Process to calculate determinant
    void calculateDeterminant() {
        int size = n.read();
        
        // Copy matrix to internal storage
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                a[i][j] = matrix[i][j].read();
            }
        }
        
        double det = 1.0;
        for (int i = 0; i < size; i++) {
            int pivot = i;
            for (int j = i + 1; j < size; j++) {
                if (fabs(a[j][i]) > fabs(a[pivot][i])) {
                    pivot = j;
                }
            }
            if (pivot != i) {
                std::swap(a[i], a[pivot]);
                det *= -1;
            }
            if (a[i][i] == 0) {
                det = 0;
                break;
            }
            det *= a[i][i];
            for (int j = i + 1; j < size; j++) {
                double factor = a[j][i] / a[i][i];
                for (int k = i + 1; k < size; k++) {
                    a[j][k] -= factor * a[i][k];
                }
            }
        }
        determinant.write(det);
    }
    
    // Constructor to register the process
    SC_CTOR(DeterminantCalculator) {
        SC_METHOD(calculateDeterminant);
        sensitive << n;
        for (int i = 0; i < 105; i++) {
            for (int j = 0; j < 105; j++) {
                sensitive << matrix[i][j];
            }
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with DeterminantCalculator
    sc_signal<int> n;
    sc_signal<double> matrix[105][105];
    sc_signal<double> det;
    
    // Instance of DeterminantCalculator
    DeterminantCalculator calc;
    
    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize matrix size
        n = 4;
        
        // Initialize matrix
        matrix[0][0] = 1; matrix[0][1] = 0; matrix[0][2] = 2; matrix[0][3] = -1;
        matrix[1][0] = 3; matrix[1][1] = 0; matrix[1][2] = 0; matrix[1][3] = 5;
        matrix[2][0] = 2; matrix[2][1] = 1; matrix[2][2] = 4; matrix[2][3] = -3;
        matrix[3][0] = 1; matrix[3][1] = 0; matrix[3][2] = 5; matrix[3][3] = 0;
        
        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);
        
        // Print the determinant
        cout << "Determinant = " << det.read() << endl;
    }
    
    // Constructor to register the process
    SC_CTOR(Testbench) : calc("calc") {
        // Connecting signals to the calculator ports
        calc.n(n);
        for (int i = 0; i < 105; i++) {
            for (int j = 0; j < 105; j++) {
                calc.matrix[i][j](matrix[i][j]);
            }
        }
        calc.determinant(det);
        
        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
