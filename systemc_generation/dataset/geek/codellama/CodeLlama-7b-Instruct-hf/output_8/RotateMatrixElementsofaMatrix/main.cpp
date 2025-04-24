
#include <systemc.h>


SC_MODULE(MatrixRotator) {

    // Interface for input 2D array
    sc_in<int> in; 
    int m, c;

    // Interface for rotated output 2D array
    sc_out<int> out; 

    // Constructor
    SC_CTOR(MatrixRotator) {
        // Process to perform rotation
        SC_METHOD(rotation);
        sensitive << in;
    }

    // Method to rotate the matrix
    void rotation() {
        int row = 0, col = 0; 
        int prev, curr; 

        // Variable to check if rotation is complete
        int has_rotated = 0;

        while (has_rotated == 0) {
            if (row + 1 == m || col + 1 == c) {  // Check if rotation is complete
                has_rotated = 1;
           }
    
           // Move elements from last row to first column
           for (int i=0; i < c; i++) {
               prev = in[row][i][0]; 
               for (int j=0; j < m; j++) {
                   out[i][j][0] = prev; 
                   prev = in[j][i][0];  // Update previous element
               } 
           }
    
           // Move elements from last column to first row
           for (int i=0; i < m; i++) {
               prev = in[i][c-1][0]; 
               for (int j=0; j < c-1; j++) {
                   out[i][j][0] = prev; 
                   prev = in[i][j][0];  // Update previous element
               } 
           }
    
           // Update row and column indices
           row++;
           col++;
        }
    }
};


SC_MODULE(Testbench) {
    // Using static const for matrix dimensions instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    
    // Signals to connect to the MatrixRotator ports
    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];

    MatrixRotator* matrixRotator;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixRotator module
        matrixRotator = new MatrixRotator("matrixRotator");
        // Connect the signals
        for (int i = 0; i < SIZE; i++) {
            matrixRotator->in[i](in[i]);
            matrixRotator->out[i](out[i]);
        }
        SC_THREAD(run_tests);
    }

    // Thread to initialize the matrix, check outputs, and assert correctness
    void run_tests() {
        // Initialize the matrix with test values:
        // { 1,  2,  3,  4,
        //   5,  6,  7,  8,
        //   9, 10, 11, 12,
        //  13, 14, 15, 16 }
        int init[SIZE] = {
            1, 2, 3, 4,
            5, 6, 7, 8,
            9, 10, 11, 12,
            13, 14, 15, 16
        };
        for (int i = 0; i < SIZE; i++) {
            in[i].write(init[i]);
        }

        // Wait for the signals to propagate
        wait(1, SC_NS);

        // Expected rotated matrix is:
        //  5   1   2   3
        //  9  10   6   4
        // 13  11   7   8
        // 14  15  16  12
        int expected[SIZE] = {
            5, 1, 2, 3,
            9, 10, 6, 4,
            13, 11, 7, 8,
            14, 15, 16, 12
        };

        // Check the output matrix and print the result
        std::cout << "Rotated Matrix:" << std::endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int index = i * N + j;
                int result = out[index].read();
                std::cout << result << " ";
                // Validate the result using assert
                assert(result == expected[index]);
            }
            std::cout << std::endl;
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // End the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench
    sc_start();         // Start the simulation
    return 0;
}
