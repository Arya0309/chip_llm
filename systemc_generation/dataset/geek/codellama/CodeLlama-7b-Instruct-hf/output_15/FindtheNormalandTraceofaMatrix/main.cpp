
#include <systemc.h>

// Module that implements findTrace() function

SC_MODULE(MatrixOperations) {
    sc_in<int> matrix[5][5]; // Input port for Matrix 
    sc_out<int> trace; // Output port for trace 

    // Constructor
    SC_CTOR(MatrixOperations) {
        // Process to perform findTrace()
        SC_METHOD(findTrace);
        sensitive << matrix << trace;
    }

    // Method to find the trace of a matrix
    void findTrace() {
        int sum = 0;
        for (int i = 0; i < 5; i++)
            sum += matrix[i][i];
        trace.write(sum);
    }
};

// Module that implements findNormal() function

SC_MODULE(FindNormal) {
    sc_in<int> matrix[5][5]; // Input port for Matrix 
    sc_out<int> normal; // Output port for normal 

    // Constructor
    SC_CTOR(FindNormal) {
        // Process to perform findNormal()
        SC_METHOD(findNormal);
        sensitive << matrix << normal;
    }

    // Method to find the normal of a matrix
    void findNormal() {
        int sum = 0;
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
                sum += matrix[i][j] * matrix[i][j];
        normal.write(sqrt(sum));
    }
};

// Module that ties the findTrace() and findNormal() functions together

SC_MODULE(FindAll) {
    sc_in<int> matrix[5][5]; // Input port for Matrix 
    sc_out<int> trace; // Output port for trace 
    sc_out<int> normal; // Output port for normal 

    // Constructor
    SC_CTOR(FindAll) {
        // Create MatrixOperations and FindNormal objects
        // - Tie their respective input and output ports to this module's ports
        // - Declare them as sc_module pointers
        MatrixOperations findTrace;
        FindNormal findNormal;

        trace << findTrace.trace;
        normal << findNormal.normal;
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
