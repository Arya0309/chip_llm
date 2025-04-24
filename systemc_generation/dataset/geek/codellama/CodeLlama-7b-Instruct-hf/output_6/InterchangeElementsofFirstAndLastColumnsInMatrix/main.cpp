
#include <systemc.h> 
#include <stdio.h>


// Define the number of rows and columns in the input matrix as macros: 
#define ROWS 4 
#define COLS 4

// Define input and output ports for the adapter submodule: 
sc_in<int> in[ROWS][COLS]; // Adapter input
sc_out<int> out[ROWS][COLS]; // Adapter output

// Define a data type to describe a 2D array of integers: 
typedef int MY_TYPE[ROWS][COLS]; 

// Define the adapter submodule: 

SC_MODULE(MatrixSwapper) { 
    // Define an input and output port for the adapter: 
    sc_in<MY_TYPE> in; 
    sc_out<MY_TYPE> out; 

    // Define a method to interchange the first and last rows of the input matrix: 
    void interchangeFirstAndLast(MY_TYPE* inputMatrix) {  
	 for (int i = 0; i < COLS; i++) {   // For each column of the input matrix... 
	  int temp = inputMatrix[0][i];     // Store the entry at the first row and column of the input matrix... 
	  inputMatrix[0][i] = inputMatrix[ROWS-1][i]; // At the last row and column of the input matrix... 
	  inputMatrix[ROWS-1][i] = temp;              // in the input matrix... 
	} 
    } 

    // Define a method to swop the first and last rows of the input matrix: 
    void swopFirstAndLast(MY_TYPE* inputMatrix) {  
    	for (int i = 0; i < COLS; i++) {   // For each column of the input matrix... 
		int temp = inputMatrix[i]; // Store the entry at the first row and column of the input matrix. 
		inputMatrix[i] = inputMatrix[ROWS-1][i]; // At the last row and column of the input matrix... 
		inputMatrix[ROWS-1][i] = temp;         // in the input matrix... 
    } 
} 

// Define a method to output the swapped matrix: 
void printMatrix(MY_TYPE* inputMatrix) { 
    for (int i = 0; i < ROWS; i++) { // For each row... 
        for (int j = 0; j < COLS; j++) { // For each column... 
            printf("%d ", inputMatrix[i][j]);  // Print the entry at the row and column specified. 
        } 
        printf("\n"); 
    } 
} 

// Define the top-level module: 

SC_MODULE(my_top) { 
    // Define an instance of the adapter submodule: 
    MatrixSwapper adapter; 

    // Define a clock signal: 
    sc_clock clk{"clk", 10, SC_NS}; 

    // Define an input port for the adapter: 
    sc_in<MY_TYPE> in; 

    // Define an output port for the adapter: 
    sc_out<MY_TYPE> out; 

    // Define a constructor for the top-level module: 
    my_top(sc_module_name name) : sc_module(name), adapter("adapter"), clk("clk"), in("in"), out("out") {} 

    // Define a method that instantiates the submodule: 
    void build() { 
        adapter.interchangeFirstAndLast(in, out); 
        adapter.swopFirstAndLast(out); 
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
