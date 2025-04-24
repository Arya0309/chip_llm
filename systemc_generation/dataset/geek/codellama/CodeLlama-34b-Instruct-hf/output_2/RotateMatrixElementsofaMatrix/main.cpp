c++
#include <systemc.h>


SC_MODULE(MatrixRotator)
{
    // Input signals
    sc_in<int> in[SIZE][SIZE];  // matrix input
    sc_in<bool> rotation_request;  // request signal
    sc_in<sc_uint<4>> num_of_rotations;  // number of rotations

    // Output signals
    sc_out<int> out[SIZE][SIZE];  // matrix output
    sc_out<bool> ready;  // ready signal

    // Variables
    int row, col, prev, curr, m, n;

    // Function to rotate a matrix
    void rotate_matrix()
    {
        // Get the dimensions of the matrix
        m = R;
        n = C;

        // Initialize the loop variables
        row = 0;
        col = 0;

        // Rotate the matrix until all rows and columns have been processed
        while (row < m && col < n)
        {
            // Check if the matrix is a square matrix
            if (row + 1 == m || col + 1 == n)
                break;

            // Rotate the first row
            prev = in[row + 1][col];
            for (int i = col; i < n; i++)
            {
                curr = in[row][i];
                out[row++][i] = prev;
                prev = curr;
            }

            // Rotate the last column
            for (int i = row; i < m; i++)
            {
                curr = in[i][n - 1];
                out[i][n - 1] = prev;
                prev = curr;
            }
            n--;

            // Rotate the last row
            if (row < m)
            {
                for (int i = n - 1; i >= col; i--)
                {
                    curr = in[m - 1][i];
                    out[m - 1][i] = prev;
                    prev = curr;
                }
            }
            m--;

            // Rotate the first column
            if (col < n)
            {
                for (int i = m - 1; i >= row; i--)
                {
                    curr = in[i][col];
                    out[i][col] = prev;
                    prev = curr;
                }
            }
            col++;
        }

        // Set the ready signal
        ready = SC_TRUE;
    }

    // Constructor
    SC_CTOR(MatrixRotator)
    {
        SC_METHOD(rotate_matrix);
        sensitive << rotation_request;
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
