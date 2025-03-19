
#include <systemc.h>

// Module to interchange first and last row of a matrix
SC_MODULE(MatrixInterchanger) {
    // Signal to indicate start of processing
    sc_in<bool> start;
    
    // Signals for matrix elements
    sc_signal<sc_uint<4>> matrix[4][4];
    
    // Process to interchange first and last row
    void interchangeRows() {
        wait(start.posedge_event()); // Wait for start signal
        
        // Interchanging elements of first and last row
        for (int i = 0; i < 4; ++i) {
            sc_uint<4> temp = matrix[0][i].read();
            matrix[0][i].write(matrix[3][i].read());
            matrix[3][i].write(temp);
        }
        
        // Indicate completion (for simulation purposes)
        start.write(false);
    }

    // Constructor to register the process
    SC_CTOR(MatrixInterchanger) {
        SC_METHOD(interchangeRows);
        sensitive << start.posedge_event();
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MatrixInterchanger
    sc_signal<bool> startSignal;
    sc_signal<sc_uint<4>> matrix[4][4];

    // Instance of MatrixInterchanger
    MatrixInterchanger interchanger;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the matrix
        matrix[0][0] = 8; matrix[0][1] = 9; matrix[0][2] = 7; matrix[0][3] = 6;
        matrix[1][0] = 4; matrix[1][1] = 7; matrix[1][2] = 6; matrix[1][3] = 5;
        matrix[2][0] = 3; matrix[2][1] = 2; matrix[2][2] = 1; matrix[2][3] = 8;
        matrix[3][0] = 9; matrix[3][1] = 9; matrix[3][2] = 7; matrix[3][3] = 7;

        // Start the interchanging process
        startSignal = true;
        wait(1, SC_NS); // Wait for one delta cycle

        // Print the modified matrix
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                cout << matrix[i][j].read() << " ";
            }
            cout << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : interchanger("interchanger") {
        // Connecting signals to the interchanger
        interchanger.start(startSignal);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                interchanger.matrix[i][j](matrix[i][j]);
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
