
#include <systemc.h>

// Function to compute the trace of a matrix
void findTrace(sc_in<int> matrix[5][5], sc_out<int> trace) {
  // Sum all the diagonal elements of the matrix
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (i == j) {
        sum += matrix[i][j];
      }
    }
  }

  // Print the trace to the output port
  trace.write(sum);
}

// Function to compute the normal of a matrix
void findNormal(sc_in<int> matrix[5][5], sc_out<int> norm) {
  // Sum all the square of all the elements of the matrix
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      sum += matrix[i][j] * matrix[i][j];
    }
  }

  // Print the normal to the output port
  norm.write(sqrt(sum));
}

// Module that wraps the two functions

SC_MODULE(MatrixOperations) {
  sc_in<int> matrix[5][5];
  sc_out<int> trace;
  sc_out<int> norm;

  // Constructor
  SC_CTOR(MatrixOperations) {
    // Calls the functions to compute the trace and normal
    SC_METHOD(findTrace);
    sensitive << matrix;
    SC_METHOD(findNormal);
    sensitive << matrix;
  }

  // Functions to compute the trace and normal
  void findTrace() {
    // Sum all the diagonal elements of the matrix
    int sum = 0;
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 5; j++) {
        if (i == j) {
          sum += matrix[i][j];
        }
      }
    }
    // Print the trace to the output port
    trace.write(sum);
  }

  void findNormal() {
    // Sum all the square of all the elements of the matrix
    int sum = 0;
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 5; j++) {
        sum += matrix[i][j] * matrix[i][j];
      }
    }
    // Print the normal to the output port
    norm.write(sqrt(sum));
  }
};


SC_MODULE(Testbench) {
    // Use static const for constants
    static const int N = 5;        // Dimension of the matrix (5x5)
    static const int SIZE = N * N; // Total number of matrix elements   
    // Signals to connect with the MatrixOperations module
    sc_signal<int> matrix[SIZE];
    sc_signal<int> trace_signal;
    sc_signal<int> norm_signal;

    // Instance of the MatrixOperations module
    MatrixOperations* matrixOps;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixOperations module
        matrixOps = new MatrixOperations("matrixOps");

        // Connect the matrix signals to the module ports
        for (int i = 0; i < SIZE; i++) {
            matrixOps->matrix[i](matrix[i]);
        }
        matrixOps->trace(trace_signal);
        matrixOps->norm(norm_signal);

        // Create a thread to run test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes the matrix, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize the matrix with values as defined in the original C++ code:
        // Matrix:
        //   {1, 1, 1, 1, 1},
        //   {2, 2, 2, 2, 2},
        //   {3, 3, 3, 3, 3},
        //   {4, 4, 4, 4, 4},
        //   {5, 5, 5, 5, 5}
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                matrix[index].write(row + 1); // Set all elements in a row to row+1
            }
        }

        // Wait for the combinational logic to settle
        wait(1, SC_NS);

        // Expected outputs:
        // Trace = 1 + 2 + 3 + 4 + 5 = 15
        // Normal = sqrt(5*(1^2 + 2^2 + 3^2 + 4^2 + 5^2)) = sqrt(275) ≈ 16 (when truncated)
        int expected_trace = 15;
        int expected_norm = static_cast<int>(sqrt(275));

        std::cout << "Computed Trace: " << trace_signal.read() << std::endl;
        std::cout << "Computed Normal: " << norm_signal.read() << std::endl;

        // Validate the computed outputs using assertions
        assert(trace_signal.read() == expected_trace);
        assert(norm_signal.read() == expected_norm);

        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // End the simulation
    }
};

// sc_main: the entry point of the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench
    sc_start();         // Start the simulation
    return 0;
}
