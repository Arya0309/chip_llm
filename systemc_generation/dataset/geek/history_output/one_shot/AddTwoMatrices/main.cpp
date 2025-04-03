
#include <systemc.h>

// Define the size of the matrices
#define N 4

// Module that adds two matrices
SC_MODULE(MatrixAdder) {
    sc_in<sc_matrix<int, N, N>> A;  // Input port for matrix A
    sc_in<sc_matrix<int, N, N>> B;  // Input port for matrix B
    sc_out<sc_matrix<int, N, N>> C; // Output port for matrix C

    // Constructor
    SC_CTOR(MatrixAdder) {
        // Process to perform matrix addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two matrices
    void add() {
        sc_matrix<int, N, N> matA = A.read();
        sc_matrix<int, N, N> matB = B.read();
        sc_matrix<int, N, N> matC;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matC(i, j) = matA(i, j) + matB(i, j);
            }
        }

        C.write(matC);
    }
};

// Custom data type for matrix
template<typename T, int ROWS, int COLS>
class sc_matrix : public sc_object {
public:
    T data[ROWS][COLS];

    sc_matrix(sc_module_name name) : sc_object(name) {}

    T& operator()(int row, int col) {
        return data[row][col];
    }

    const T& operator()(int row, int col) const {
        return data[row][col];
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<sc_matrix<int, N, N>> A; // Signal for matrix A
    sc_signal<sc_matrix<int, N, N>> B; // Signal for matrix B
    sc_signal<sc_matrix<int, N, N>> C; // Signal for matrix C

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
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A.write()[i][j] = i + 1;
                B.write()[i][j] = i + 1;
            }
        }

        // Wait for the adder to process
        wait(1, SC_NS);

        // Read and print the result matrix C
        sc_matrix<int, N, N> result = C.read();
        cout << "Result matrix is " << endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << result(i, j) << " ";
            }
            cout << endl;
        }

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
