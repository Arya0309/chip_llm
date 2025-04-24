
#include <systemc.h>

// Module for swapping the first and last rows of a matrix

SC_MODULE(MatrixSwapper) {
    sc_in<int> in[n]; // Input port for the matrix
    sc_out<int> out[n]; // Output port for the matrix

    // Constructor
    SC_CTOR(MatrixSwapper) {
        // Process to perform row swapping
        SC_METHOD(rowSwap);
        sensitive << in;
    }

    // Method to swap the first and last rows of the matrix
    void rowSwap() {
        int rows = in[0].size();
        int temp = in[0];
        out[0] = in[rows - 1];
        out[rows - 1] = temp;
    }
};

// Testbench code to drive the MatrixSwapper module


SC_MODULE(tb) {
    // Initialize the input matrix
    sc_in<int> in[n];
    sc_out<int> out[n];
    MatrixSwapper rs("rs");

    SC_METHOD(doTest) {
        int m[n][n] = {{8, 9, 7, 6},
                       {4, 7, 6, 5},
                       {3, 2, 1, 8},
                       {9, 9, 7, 7}};
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                in[i][j] = m[i][j];
            }
        }
        rs.in.write(in);
        rs.run();
        out = rs.out.read();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                std::cout << out[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};



SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    
    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];
    MatrixSwapper* swapper;

    SC_CTOR(Testbench) {
        swapper = new MatrixSwapper("swapper");
        // Connect the signals to the MatrixSwapper ports
        for (int i = 0; i < SIZE; i++) {
            swapper->in[i](in[i]);
            swapper->out[i](out[i]);
        }
        SC_THREAD(run_tests);
    }

    // Thread that runs the test, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize the matrix as in the original C++ code:
        //   { {8, 9, 7, 6},
        //     {4, 7, 6, 5},
        //     {3, 2, 1, 8},
        //     {9, 9, 7, 7} }
        int m[SIZE] = {8, 9, 7, 6,
                       4, 7, 6, 5,
                       3, 2, 1, 8,
                       9, 9, 7, 7};
        for (int i = 0; i < SIZE; i++) {
            in[i].write(m[i]);
        }

        // Wait for signals to propagate
        wait(1, SC_NS);

        // Expected output after swapping first and last rows:
        //   { {9, 9, 7, 7},
        //     {4, 7, 6, 5},
        //     {3, 2, 1, 8},
        //     {8, 9, 7, 6} }
        int expected[SIZE] = {9, 9, 7, 7,
                              4, 7, 6, 5,
                              3, 2, 1, 8,
                              8, 9, 7, 6};

        // Print and check the result matrix
        std::cout << "Swapped Matrix:" << std::endl;
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int idx = row * N + col;
                int result = out[idx].read();
                std::cout << result << " ";
                assert(result == expected[idx]);
            }
            std::cout << std::endl;
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench
    sc_start();         // Start the simulation
    return 0;
}
