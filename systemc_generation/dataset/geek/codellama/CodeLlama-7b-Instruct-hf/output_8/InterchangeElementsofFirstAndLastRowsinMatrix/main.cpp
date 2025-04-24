scss
#include <systemc.h>

// Module to swap the first and last column of a matrix

SC_MODULE(MatrixSwapper) {
    sc_in<int> in[SIZE][n]; // Input port for the input matrix
    sc_out<int> out[SIZE][n]; // Output port for the output matrix

    // Constructor
    SC_CTOR(MatrixSwapper) : out("out") {
        // Process to swap the columns
        SC_METHOD(swap);
        sensitive << in;
    }

    // Method to swap the first and last columns
    void swap() {
        for (int i = 0; i < n; i++) {
            // Swap the first and last columns
            out[i][0] = in[i][n-1];
            out[i][n-1] = in[i][0];
        }
    }
};

// Testbench for the MatrixSwapper module
int main() {
    // Create the module and its inputs and outputs
    MatrixSwapper MatrixSwapperInst(in, out);

    // Initialize the input matrix
    int m[n][n] = {{8, 9, 7, 6}, 
                    {4, 7, 6, 5}, 
                    {3, 2, 1, 8}, 
                    {9, 9, 7, 7}};

    // Apply the MatrixSwapper function on the input matrix
    MatrixSwapperInst.swap();

    // Print the resulting matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << out[i][j] << " ";
        }
        cout << endl;
    }
}


SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;

    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];

    MatrixSwapper* matrixSwapper;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixSwapper module
        matrixSwapper = new MatrixSwapper("matrixSwapper");
        // Connect the testbench signals to the module's ports
        for (int i = 0; i < SIZE; i++) {
            matrixSwapper->in[i](in[i]);
            matrixSwapper->out[i](out[i]);
        }
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Define the input matrix as per the original C++ code:
        // { {8, 9, 7, 6},
        //   {4, 7, 6, 5},
        //   {3, 2, 1, 8},
        //   {9, 9, 7, 7} }
        int input_matrix[SIZE] = {
            8, 9, 7, 6,
            4, 7, 6, 5,
            3, 2, 1, 8,
            9, 9, 7, 7
        };

        // Expected output after swapping first and last columns:
        // { {6, 9, 7, 8},
        //   {5, 7, 6, 4},
        //   {8, 2, 1, 3},
        //   {7, 9, 7, 9} }
        int expected[SIZE] = {
            6, 9, 7, 8,
            5, 7, 6, 4,
            8, 2, 1, 3,
            7, 9, 7, 9
        };

        // Write input matrix values into signals
        for (int i = 0; i < SIZE; i++) {
            in[i].write(input_matrix[i]);
        }

        // Allow the signals to propagate
        wait(1, SC_NS);

        // Check the swapped matrix and print the result
        std::cout << "Swapped Matrix:" << std::endl;
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                int result = out[index].read();
                // Assert that the result matches the expected value
                assert(result == expected[index]);
                std::cout << result << " ";
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
