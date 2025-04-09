
#include <systemc.h>

// Define constants
static const int N = 4;
static const int SIZE = N * N;

// SystemC module to rotate a matrix

SC_MODULE(MatrixRotator) {
    // Input ports for the matrix (flattened as 1D array)
    sc_in<int> in[SIZE];
    // Output port for the rotated matrix (flattened as 1D array)
    sc_out<int> out[SIZE];

    SC_CTOR(MatrixRotator) {
        // Sensitive to changes in input ports
        SC_METHOD(do_rotate);
        for (int i = 0; i < SIZE; i++) {
            sensitive << in[i];
        }
    }

    // Method to perform matrix rotation
    void do_rotate() {
        int m = N, n = N;
        int row = 0, col = 0;
        int prev, curr;
        
        while (row < m && col < n) {
            if (row + 1 == m || col + 1 == n) break;

            prev = in[(row + 1) * N + col];

            for (int i = col; i < n; i++) {
                curr = in[row * N + i];
                in[row * N + i] = prev;
                prev = curr;
            }
            row++;

            for (int i = row; i < m; i++) {
                curr = in[i * N + n - 1];
                in[i * N + n - 1] = prev;
                prev = curr;
            }
            n--;

            if (row < m) {
                for (int i = n - 1; i >= col; i--) {
                    curr = in[(m - 1) * N + i];
                    in[(m - 1) * N + i] = prev;
                    prev = curr;
                }
            }
            m--;

            if (col < n) {
                for (int i = m - 1; i >= row; i--) {
                    curr = in[i * N + col];
                    in[i * N + col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Copy the rotated matrix to the output ports
        for (int i = 0; i < SIZE; i++) {
            out[i].write(in[i].read());
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
