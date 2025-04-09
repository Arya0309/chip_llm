
   #include <systemc.h>

   
SC_MODULE(MatrixSwapper) {
       sc_in<int> in[4][4];  // Input ports for the matrix
       sc_out<int> out[4][4]; // Output ports for the matrix

       // Constructor
       SC_CTOR(MatrixSwapper) {
           // Process to perform row swapping
           SC_METHOD(swapRows);
           sensitive << in[0][0] << in[0][1] << in[0][2] << in[0][3]
                     << in[1][0] << in[1][1] << in[1][2] << in[1][3]
                     << in[2][0] << in[2][1] << in[2][2] << in[2][3]
                     << in[3][0] << in[3][1] << in[3][2] << in[3][3];
       }

       // Method to swap the first and last rows of the matrix
       void swapRows() {
           for (int i = 0; i < 4; i++) {
               // Swap elements of the first and last rows
               out[0][i].write(in[3][i].read());
               out[3][i].write(in[0][i].read());

               // Copy other rows as they are
               for (int j = 1; j < 3; j++) {
                   out[j][i].write(in[j][i].read());
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
