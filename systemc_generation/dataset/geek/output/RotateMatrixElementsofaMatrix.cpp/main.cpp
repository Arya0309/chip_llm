
#include <systemc.h>

// Define constants for matrix dimensions
const int R = 4;
const int C = 4;

// SystemC module to rotate a matrix
SC_MODULE(MatrixRotator) {
    // Port to trigger the rotation
    sc_in<bool> start;

    // Signal to indicate completion of rotation
    sc_out<bool> done;

    // Internal matrix storage
    int mat[R][C];

    // Constructor to initialize the matrix and register the process
    SC_CTOR(MatrixRotator) {
        SC_METHOD(rotatematrix);
        sensitive << start;
        dont_initialize();
        done.write(false);
    }

    // Method to perform matrix rotation
    void rotatematrix() {
        int row = 0, col = 0;
        int prev, curr;

        while (row < R && col < C) {
            if (row + 1 == R || col + 1 == C)
                break;

            prev = mat[row + 1][col];

            // Rotate first row
            for (int i = col; i < C; i++) {
                curr = mat[row][i];
                mat[row][i] = prev;
                prev = curr;
            }
            row++;

            // Rotate last column
            for (int i = row; i < R; i++) {
                curr = mat[i][C-1];
                mat[i][C-1] = prev;
                prev = curr;
            }
            C--;

            // Rotate last row
            if (row < R) {
                for (int i = C-1; i >= col; i--) {
                    curr = mat[R-1][i];
                    mat[R-1][i] = prev;
                    prev = curr;
                }
            }
            R--;

            // Rotate first column
            if (col < C) {
                for (int i = R-1; i >= row; i--) {
                    curr = mat[i][col];
                    mat[i][col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Print rotated matrix
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++)
                cout << mat[i][j] << " ";
            cout << endl;
        }

        done.write(true);
    }

    // Function to set the initial matrix
    void setMatrix(int matrix[R][C]) {
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                mat[i][j] = matrix[i][j];
            }
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to trigger the rotation
    sc_signal<bool> start;

    // Signal to indicate completion of rotation
    sc_signal<bool> done;

    // Instance of MatrixRotator
    MatrixRotator rotator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize matrix
        int a[R][C] = {{1, 2, 3, 4},
                       {5, 6, 7, 8},
                       {9, 10, 11, 12},
                       {13, 14, 15, 16}};
        rotator.setMatrix(a);

        // Trigger rotation
        start.write(true);
        wait(1, SC_NS);
        start.write(false);

        // Wait for rotation to complete
        wait(done.posedge_event());

        // Print completion message
        cout << "Matrix rotation completed." << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : rotator("rotator") {
        // Connect signals to the rotator ports
        rotator.start(start);
        rotator.done(done);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
