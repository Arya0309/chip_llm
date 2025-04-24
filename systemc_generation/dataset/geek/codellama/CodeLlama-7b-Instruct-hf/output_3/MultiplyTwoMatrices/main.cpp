
if (C1 != R2) { // MAJOR LINE OF CODE
	cout << "The number of columns in Matrix-1 must "
			"be equal to the number of rows in "
			"Matrix-2"
		<< endl;
	cout << "Please update MACROs according to your "
			"array dimension in #define section"
		<< endl;
	exit(EXIT_FAILURE); // MAJOR LINE OF CODE
}

// Function call
mulMat(mat1, mat2);

return 0;
}

// SystemC code equivalent to the C++ program
#include <systemc.h> // library for SystemC design

// C++/C data types removed for brevity

// Functional Block Diagram of the multiplier

SC_MODULE(MatrixMultiplier) {

// Matrix A, B, and C
sc_in<int> A[R1][C1]; // input port for matrix A
sc_in<int> B[R2][C2]; // input port for matrix B
sc_out<int> C[R1][C2]; // output port for matrix C

// Constructor
SC_CTOR(Adder) {
	// sensitivity list stating what
	// elements should trigger a new calculation
	// MAJOR BLOCK OF CODE
	sensitive << A << B;

	// generating R1*C2 modules, each one that
	// performs a dot product between a row
	// of A and a column of B, storing the result
	// in C
	for (int i = 0; i < R1; i++) {
		// for each i, generate R2*C2 modules
		for (int j = 0; j < C2; j++) {
			// for each j, generate R2*1 modules
			for (int k = 0; k < R2; k++) {
				// for each k, C(i,j) = C(i,j) + A(i,k)*B(k,j)
				C(i, j) += A(i, k) * B(k, j);
			} // MAJOR COMMENT

			// write C(i,j) to port C
			C.write(C(i, j));
		} // MAJOR COMMENT
	} // MAJOR COMMENT
} // SC_CTOR

// MAIN MODULE
int main() {
	// MAJOR LINE OF CODE
	MatrixMultiplier mult(A, B, C); // call function

	return 0;
}



SC_MODULE(Testbench) {
    // Use static const for matrix dimensions
    static const int R1 = 2;      // Rows in Matrix-1
    static const int C1 = 2;      // Columns in Matrix-1
    static const int R2 = 2;      // Rows in Matrix-2
    static const int C2 = 3;      // Columns in Matrix-2

    static const int SIZE_A = R1 * C1;      // Total elements in Matrix-1
    static const int SIZE_B = R2 * C2;      // Total elements in Matrix-2
    static const int SIZE_RESULT = R1 * C2; // Total elements in the Result matrix
    
    // Signals to connect to the MatrixMultiplier ports
    sc_signal<int> A[SIZE_A];
    sc_signal<int> B[SIZE_B];
    sc_signal<int> C[SIZE_RESULT];

    MatrixMultiplier* mm;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixMultiplier module
        mm = new MatrixMultiplier("mm");
        // Connect signals to the module's ports
        for (int i = 0; i < SIZE_A; i++) {
            mm->A[i](A[i]);
        }
        for (int i = 0; i < SIZE_B; i++) {
            mm->B[i](B[i]);
        }
        for (int i = 0; i < SIZE_RESULT; i++) {
            mm->C[i](C[i]);
        }
        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes inputs, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize Matrix A (2x2):
        // { {1, 1},
        //   {2, 2} }
        A[0].write(1); // Row 0, Column 0
        A[1].write(1); // Row 0, Column 1
        A[2].write(2); // Row 1, Column 0
        A[3].write(2); // Row 1, Column 1

        // Initialize Matrix B (2x3):
        // { {1, 1, 1},
        //   {2, 2, 2} }
        B[0].write(1); // Row 0, Column 0
        B[1].write(1); // Row 0, Column 1
        B[2].write(1); // Row 0, Column 2
        B[3].write(2); // Row 1, Column 0
        B[4].write(2); // Row 1, Column 1
        B[5].write(2); // Row 1, Column 2

        // Wait for the signals to propagate
        wait(1, SC_NS);

        // Expected result after multiplication:
        // For result[0][0]: 1*1 + 1*2 = 3
        // For result[0][1]: 1*1 + 1*2 = 3
        // For result[0][2]: 1*1 + 1*2 = 3
        // For result[1][0]: 2*1 + 2*2 = 6
        // For result[1][1]: 2*1 + 2*2 = 6
        // For result[1][2]: 2*1 + 2*2 = 6
        int expected[SIZE_RESULT] = {3, 3, 3, 6, 6, 6};

        std::cout << "Matrix multiplication result:" << std::endl;
        for (int i = 0; i < SIZE_RESULT; i++) {
            int res = C[i].read();
            std::cout << res << " ";
            // Assert the computed result matches the expected value
            assert(res == expected[i]);
        }
        std::cout << std::endl;
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench
    sc_start();         // Start simulation
    return 0;
}
