
#include <systemc.h>

// Module that adds two 4x4 matrices
SC_MODULE(MatrixAdder) {
    sc_in<sc_matrix<int>> A;  // Input port for the first matrix
    sc_in<sc_matrix<int>> B;  // Input port for the second matrix
    sc_out<sc_matrix<int>> C; // Output port for the result matrix

    // Constructor
    SC_CTOR(MatrixAdder) {
        // Process to perform matrix addition
        SC_METHOD(add_matrices);
        sensitive << A << B;
    }

    // Method to add the two matrices
    void add_matrices() {
        sc_matrix<int> A_val = A.read();
        sc_matrix<int> B_val = B.read();
        sc_matrix<int> C_val(4, 4); // Result matrix

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                C_val(i, j) = A_val(i, j) + B_val(i, j);
            }
        }
        C.write(C_val);
    }
};

// Custom data type for 4x4 matrix
typedef sc_fixed<16, 8> fixed_point_t;
typedef sc_matrix<fixed_point_t, 4, 4> sc_matrix_int;

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<sc_matrix_int> A; // Signal for the first matrix
    sc_signal<sc_matrix_int> B; // Signal for the second matrix
    sc_signal<sc_matrix_int> C; // Signal for the result matrix

    MatrixAdder matrix_adder_inst; // Instance of the MatrixAdder module

    // Constructor
    SC_CTOR(Testbench) : matrix_adder_inst("matrix_adder") {
        // Connect signals to ports
        matrix_adder_inst.A(A);
        matrix_adder_inst.B(B);
        matrix_adder_inst.C(C);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize matrices A and B
        sc_matrix_int A_val(4, 4);
        sc_matrix_int B_val(4, 4);

        // Assign values to matrix A
        A_val(0, 0) = 1; A_val(0, 1) = 1; A_val(0, 2) = 1; A_val(0, 3) = 1;
        A_val(1, 0) = 2; A_val(1, 1) = 2; A_val(1, 2) = 2; A_val(1, 3) = 2;
        A_val(2, 0) = 3; A_val(2, 1) = 3; A_val(2, 2) = 3; A_val(2, 3) = 3;
        A_val(3, 0) = 4; A_val(3, 1) = 4; A_val(3, 2) = 4; A_val(3, 3) = 4;

        // Assign values to matrix B
        B_val(0, 0) = 1; B_val(0, 1) = 1; B_val(0, 2) = 1; B_val(0, 3) = 1;
        B_val(1, 0) = 2; B_val(1, 1) = 2; B_val(1, 2) = 2; B_val(1, 3) = 2;
        B_val(2, 0) = 3; B_val(2, 1) = 3; B_val(2, 2) = 3; B_val(2, 3) = 3;
        B_val(3, 0) = 4; B_val(3, 1) = 4; B_val(3, 2) = 4; B_val(3, 3) = 4;

        // Write matrices A and B to signals
        A.write(A_val);
        B.write(B_val);

        // Wait for the matrix adder to process
        wait(1, SC_NS);

        // Read and print the result matrix C
        sc_matrix_int C_val = C.read();
        cout << "Result matrix is " << endl;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cout << C_val(i, j) << " ";
            }
            cout << endl;
        }

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
