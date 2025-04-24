
#include <systemc.h>


SC_MODULE(DeterminantCalculator) {
  // Input ports
  sc_in<double> matrix[4][4];
  sc_in<int> SIZE;
  
  // Output port
  sc_out<double> det;
  
  // Local variables
  double a[4][4];
  int n = 4;
  
  SC_CTOR(DeterminantCalculator) {
    // Process to perform row operations and update determinant
    SC_METHOD(determinant);
    sensitive << matrix << SIZE;
  }
  
  void determinant() {
    // Initialize determinant to 1.0
    det.write(1.0);
    
    // Loop over each row of the matrix
    for (int i = 0; i < n; i++) {
      // Find the pivot element
      double pivot = matrix[i][i];
      int pivot_row = i;
      for (int j = i + 1; j < n; j++) {
        if (abs(matrix[j][i]) > abs(pivot)) {
          pivot = matrix[j][i];
          pivot_row = j;
        }
      }
      
      // Check if the pivot element is zero
      if (pivot == 0) {
        det.write(0);
        return;
      }
      
      // Perform row operations to eliminate elements in the pivot column below the pivot element
      for (int j = i + 1; j < n; j++) {
        // Swap the pivot row with the current row
        swap(matrix[i], matrix[pivot_row]);
        // Subtract multiples of the pivot row from the other rows
        double factor = matrix[j][i] / pivot;
        for (int k = i + 1; k < n; k++) {
          matrix[j][k] -= matrix[i][k] * factor;
        }
      }
      
      // Update the determinant by multiplying it by the pivot element
      det *= pivot;
    }
  }
};


SC_MODULE(Testbench) {
    // Use static const for constants
    static const int N = 4;
    static const int SIZE = N * N;
    
    sc_signal<double> matrix[SIZE];
    sc_signal<double> det;

    DeterminantCalculator* detCalc;

    SC_CTOR(Testbench) {
        detCalc = new DeterminantCalculator("detCalc");
        for (int i = 0; i < SIZE; i++) {
            detCalc->matrix[i](matrix[i]);
        }
        detCalc->det(det);
        SC_THREAD(run_tests);
    }

    // Thread to initialize matrix, check result, and assert correctness
    void run_tests() {
        // Initialize the 4x4 matrix (row-major order)
        // Matrix:
        // { {1, 0, 2, -1},
        //   {3, 0, 0, 5},
        //   {2, 1, 4, -3},
        //   {1, 0, 5, 0} }
        double init_matrix[SIZE] = {
            1, 0, 2, -1,
            3, 0, 0, 5,
            2, 1, 4, -3,
            1, 0, 5, 0
        };
        for (int i = 0; i < SIZE; i++) {
            matrix[i].write(init_matrix[i]);
        }

        // Wait for signals to propagate and process to complete
        wait(2, SC_NS);

        // Read the result and check
        double result = det.read();
        std::cout << "Determinant = " << result << std::endl;
        // Expected determinant is 30 (as computed in the original code)
        assert(std::abs(result - 30.0) < 1e-6);
        std::cout << "Test passed successfully" << std::endl;

        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
