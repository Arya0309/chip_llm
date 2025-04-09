
   #include <systemc.h>

   
SC_MODULE(MatrixSwapper) {
       sc_in<int> in[SIZE];  // Input port array for the matrix elements
       sc_out<int> out[SIZE]; // Output port array for the modified matrix elements

       // Constructor
       SC_CTOR(MatrixSwapper) {
           // Process to perform column swapping
           SC_METHOD(swapColumns);
           sensitive << in[0] << in[1] << in[2] << in[3] << in[4] << in[5] << in[6] << in[7] << in[8] << in[9] << in[10] << in[11] << in[12] << in[13] << in[14] << in[15];
       }

       // Method to swap the first and last columns of the matrix
       void swapColumns() {
           int matrix[4][4];
           int temp;

           // Copy input to matrix
           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 4; j++) {
                   matrix[i][j] = in[i * 4 + j].read();
               }
           }

           // Swap the first and last columns
           for (int i = 0; i < 4; i++) {
               temp = matrix[i][0];
               matrix[i][0] = matrix[i][3];
               matrix[i][3] = temp;
           }

           // Copy modified matrix to output
           for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 4; j++) {
                   out[i * 4 + j].write(matrix[i][j]);
               }
           }
       }
   };
   

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
