c
#include <systemc.h>


SC_MODULE(MatrixSwapper) {
  // Assign the inputs from the top and bottom of the array.
  sc_in<int> inputMatrix[SIZE];
  sc_in<int> lastElement;
  sc_in<int> topElement;
  sc_out<int> outputMatrix[SIZE];
  
  void interchangeFirstLast(int m[SIZE][n]); // Function to interchange the first and last row
 
  // SystemC constructor
  SC_CTOR{
    // Connect the inputs and outputs
    SUBCK(inputMatrix, =, outputMatrix[SIZE]);
    SUBCK(lastElement, =, m[SIZE-1][0]);
    SUBCK(topElement, =, m[0][0]);
  
    // Assign the arrays as the inputs and outputs
    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];
 
 Listen to the inputs to the function.
  void process_inputs();
  
  void interchangeFirstLast(int m[SIZE][n]) // Function to interchange the first and last row
  {
    // Assign the arrays as the inputs and outputs
    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];
 
    // Listen to the inputs to the function.
    void process_inputs();
  
    // Typecast the elements in the array to integers 
    int t = m[0][i];
    m[0][i] = m[rows - 1][i];
    m[rows - 1][i] = t;
  }
 
 // Driver code
 void main();
 
 void main();
 #define n 4
 
 int main()
 {
   // input in the array 
   int m[n][n] = {{8, 9, 7, 6}, 
                   {4, 7, 6, 5}, 
                   {3, 2, 1, 8}, 
                   {9, 9, 7, 7}}; 
                  
   // Perform the interchange
   interchangeFirstLast(m);
 }


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
