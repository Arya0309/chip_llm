scss
#include <systemc.h>

// Module to interchange the first and last rows of a 2D matrix

SC_MODULE(MatrixSwapper) {
    // Input signals
    sc_in<int> in[SIZE];
    
    // Output signals
    sc_out<int> out[SIZE];
    
    // Constructor
    SC_CTOR(MatrixSwapper) {
        // Process to interchange the first and last rows
        SC_METHOD(interchange);
        sensitive << in;
    }
    
    // Method to interchange the first and last rows
    void interchange() {
        int n = in.size();
        for (int i = 0; i < n; i++) {
            int t = in[0][i];
            in[0][i] = in[n - 1][i];
            in[n - 1][i] = t;
        }
    }
};

// Testbench to integrate the module with the testbench input and output signals

SC_MODULE(MatrixSwapperTestbench) {
    SC_CTOR(MatrixSwapperTestbench) {
        MatrixSwapper dut; // Instantiate the module under test

        // Connect input and output signals
        in[0] = dut.in(0);
        out[0] = dut.out(0);

        // Run the simulation
        SC_CTOR(MatrixSwapperTestbench) {
            RuntSim ("vsim"); // Use VSIM for simulation
            stopTime = 100ns; // Stop simulation after 100ns
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
