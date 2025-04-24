
#include <systemc.h>


SC_MODULE(MatrixTranspose) {

  // Input port for the matrix to be transposed
  sc_in<int> A[N];

  // Output port for the transposed matrix
  sc_out<int> B[N];

  // Function to find the transpose of a matrix
  void transpose() {
    // Iterate through the arrays
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        // Store the element at the transpose of the current element's position
        B[i][j] = A[j][i];
      }
    }
  }

  // Main method
  SC_CTOR(MatrixTranspose) {
    // Connect the input and output ports
    sc_signal<> A_signal = sc_make_signal_1d<int, N>();
    sc_signal<> B_signal = sc_make_signal_1d<int, N>();
    sc_signal<int> result_signal;

    // Connect the signals to the input and output ports
    A_signal.range() = &A[0];
    B_signal.range() = &B[0];
    result_signal.range() = &result[0];

    // Call the transpose function and store the result
    transpose(A_signal, B_signal, result_signal);
  }

  // Output the transposed matrix
  void output() {
    // Iterate through the arrays and print the elements
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        sc_out << result[j][i] << " ";
      }
      sc_out << "\n";
    }
  }
};


SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    
    // Signals to connect to the MatrixTranspose ports
    sc_signal<int> A[SIZE];
    sc_signal<int> B[SIZE];

    MatrixTranspose* transpose_inst;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixTranspose module
        transpose_inst = new MatrixTranspose("MatrixTranspose");
        // Connect the signals to the module's ports
        for (int i = 0; i < SIZE; i++) {
            transpose_inst->A[i](A[i]);
            transpose_inst->B[i](B[i]);
        }
        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes inputs, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize matrix A as defined in the original C++ code:
        // A = { {1, 1, 1, 1},
        //       {2, 2, 2, 2},
        //       {3, 3, 3, 3},
        //       {4, 4, 4, 4} }
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                A[index].write(row + 1); // row 0 -> 1, row 1 -> 2, etc.
            }
        }

        // Wait for the signals to propagate and the combinational logic to settle.
        wait(1, SC_NS);

        // Check the result transposed matrix B and print the output
        std::cout << "Result matrix is:" << std::endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int index = i * N + j;
                // Expected value: since B[i][j] = A[j][i] and each row j of A is (j+1)
                int expected = j + 1;
                int result = B[index].read();
                // Validate the result using assert
                assert(result == expected);
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
