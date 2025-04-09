
#include <systemc.h>

#define SIZE 4

// Module to interchange first and last row of a 4x4 matrix

SC_MODULE(MatrixSwapper) {
    sc_in<int> in[SIZE][SIZE];  // Input ports for the matrix elements
    sc_out<int> out[SIZE][SIZE]; // Output ports for the matrix elements

    // Constructor
    SC_CTOR(MatrixSwapper) {
        // Process to perform row interchange
        SC_METHOD(interchangeRows);
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                sensitive << in[i][j];
            }
        }
    }

    // Method to interchange the first and last row
    void interchangeRows() {
        // Temporary storage for swapping
        int temp[SIZE];

        // Store the first row in temp
        for (int i = 0; i < SIZE; ++i) {
            temp[i] = in[0][i].read();
        }

        // Write the last row to the first row output
        for (int i = 0; i < SIZE; ++i) {
            out[0][i].write(in[SIZE-1][i].read());
        }

        // Write the stored first row to the last row output
        for (int i = 0; i < SIZE; ++i) {
            out[SIZE-1][i].write(temp[i]);
        }

        // Copy the middle rows directly to the output
        for (int i = 1; i < SIZE-1; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                out[i][j].write(in[i][j].read());
            }
        }
    }
};

// Example top module to demonstrate usage

SC_MODULE(Top) {
    MatrixSwapper interchanger;

    // Input and output signals
    sc_signal<int> input[SIZE][SIZE];
    sc_signal<int> output[SIZE][SIZE];

    // Constructor
    SC_CTOR(Top) : interchanger("interchanger") {
        // Connect input signals to the interchanger
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                interchanger.in[i][j](input[i][j]);
            }
        }

        // Connect output signals from the interchanger
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                interchanger.out[i][j](output[i][j]);
            }
        }

        // Initialize the input matrix
        SC_THREAD(initializeInput);
        dont_initialize();
        sensitive << sc_start_of_simulation_event();

        // Print the output matrix
        SC_THREAD(printOutput);
        sensitive << sc_start_of_simulation_event();
    }

    // Initialize input matrix
    void initializeInput() {
        int initMatrix[SIZE][SIZE] = {
            {8, 9, 7, 6},
            {4, 7, 6, 5},
            {3, 2, 1, 8},
            {9, 9, 7, 7}
        };
        wait(1, SC_NS); // Wait for simulation to start
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                input[i][j].write(initMatrix[i][j]);
            }
        }
    }

    // Print the output matrix
    void printOutput() {
        wait(1, SC_NS); // Wait for simulation to start and processing to complete
        cout << "Modified Matrix:" << endl;
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                cout << output[i][j].read() << " ";
            }
            cout << endl;
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
