
#include <systemc.h>


SC_MODULE(MatrixComparator) {
    sc_in<sc_vector<sc_vector<int>>> A;  // Input port for the first matrix
    sc_in<sc_vector<sc_vector<int>>> B;  // Input port for the second matrix
    sc_out<bool> identical;              // Output port for the result

    // Constructor
    SC_CTOR(MatrixComparator) {
        num_rows = A.size();
        for (int i = 0; i < num_rows; i++) {
            SC_THREAD(compare_row);
            sensitive << A[i] << B[i];
            compare_row(i);
        }

        SC_THREAD(aggregate_results);
    }

    // Method to compare a single row of the matrices
    void compare_row(int row_index) {
        bool are_identical = true;
        for (int j = 0; j < A[row_index].size(); j++) {
            if (A[row_index][j] != B[row_index][j]) {
                are_identical = false;
                break;
            }
        }
        row_compare_done.write(true);
        row_result.write(are_identical);
    }

private:
    int num_rows;
    sc_signal<bool> row_compare_done;
    sc_signal<bool> row_result;
    sc_signal<int> num_completed_rows;
    sc_signal<bool> has_error;

    void aggregate_results() {
        while (true) {
            wait(row_compare_done.posedge());
            num_completed_rows.write(num_completed_rows.read() + (row_result.read() ? 1 : 0));

            // Check if any row comparison failed
            if (!row_result.read()) {
                has_error.write(true);
                break;
            }

            // Check if all rows have been compared
            if (num_completed_rows.read() == num_rows) {
                identical.write(!has_error.read());
                break;
            }
        }
    }
};


SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    sc_signal<int> A[SIZE];
    sc_signal<int> B[SIZE];
    sc_signal<bool> result;

    MatrixComparator* comparator;

    SC_CTOR(Testbench) {
        // Instantiate the MatrixComparator module
        comparator = new MatrixComparator("comparator");
        // Connect signals to the module's ports
        for (int i = 0; i < SIZE; i++) {
            comparator->A[i](A[i]);
            comparator->B[i](B[i]);
        }
        comparator->identical(result);

        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that initializes inputs, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize matrices A and B as defined in the original C++ code:
        // Matrix A:
        //   { 1, 1, 1, 1 },
        //   { 2, 2, 2, 2 },
        //   { 3, 3, 3, 3 },
        //   { 4, 4, 4, 4 }
        // Matrix B: identical to Matrix A.
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int index = row * N + col;
                A[index].write(row + 1); // For row 0, write 1; row 1, write 2; etc.
                B[index].write(row + 1);
            }
        }

        // Wait for the signals to propagate and for the comparison to be performed.
        wait(1, SC_NS);

        // Check the result and print the output
        if (result.read() == true) {
            std::cout << "Matrices are identical" << std::endl;
        } else {
            std::cout << "Matrices are not identical" << std::endl;
        }

        // Assert the result is true, as the matrices are identical.
        assert(result.read() == true);
        std::cout << "Test passed successfully" << std::endl;

        sc_stop(); // End the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench
    sc_start();         // Start the simulation
    return 0;
}
