
#include <systemc.h>

// Define the size of the matrices
#define N 4

// MatrixAdder module
SC_MODULE(MatrixAdder) {
    // Input ports for matrices A and B
    sc_in<int> A[N][N];
    sc_in<int> B[N][N];

    // Output port for matrix C
    sc_out<int> C[N][N];

    // Process to add matrices A and B
    void addMatrices() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                // Read inputs from A and B
                int a = A[i][j].read();
                int b = B[i][j].read();

                // Calculate the sum and write to C
                C[i][j].write(a + b);
            }
        }
    }

    // Constructor to register the process
    SC_CTOR(MatrixAdder) {
        SC_METHOD(addMatrices);
        sensitive << A[0][0]; // Sensitivity to one element is sufficient
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MatrixAdder
    sc_signal<int> A[N][N];
    sc_signal<int> B[N][N];
    sc_signal<int> C[N][N];

    // Instance of MatrixAdder
    MatrixAdder adder;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize matrices A and B
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + 1; // Example initialization
                B[i][j] = i + 1; // Example initialization
            }
        }

        // Wait for one delta cycle to let the adder process
        wait(1, SC_NS);

        // Print the results
        cout << "Result matrix is " << endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << C[i][j].read() << " ";
            }
            cout << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : adder("adder") {
        // Connecting signals to the adder ports
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                adder.A[i][j](A[i][j]);
                adder.B[i][j](B[i][j]);
                adder.C[i][j](C[i][j]);
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
