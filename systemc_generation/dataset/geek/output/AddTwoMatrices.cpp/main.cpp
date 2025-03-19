
#include <systemc.h>

SC_MODULE(MatrixAdder) {
    sc_in<int> clk;             // Clock signal
    sc_in<bool> rst;            // Reset signal
    sc_in<int> A[4][4];         // Input matrix A
    sc_in<int> B[4][4];         // Input matrix B
    sc_out<int> C[4][4];        // Output matrix C
    sc_in<bool> start;          // Start signal to initiate computation
    sc_out<bool> done;          // Done signal indicating completion

    SC_CTOR(MatrixAdder) {
        SC_METHOD(add_matrices);
        sensitive << clk.pos();
        dont_initialize();
    }

    void add_matrices() {
        if (rst.read()) {
            done.write(false);
        } else if (start.read()) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    C[i][j].write(A[i][j].read() + B[i][j].read());
                }
            }
            done.write(true);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS); // 10 ns period clock
    sc_signal<bool> rst;
    sc_signal<bool> start;
    sc_signal<bool> done;

    sc_signal<int> A[4][4];
    sc_signal<int> B[4][4];
    sc_signal<int> C[4][4];

    MatrixAdder matrix_adder("matrix_adder");
    matrix_adder.clk(clk);
    matrix_adder.rst(rst);
    matrix_adder.start(start);
    matrix_adder.done(done);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            matrix_adder.A[i][j](A[i][j]);
            matrix_adder.B[i][j](B[i][j]);
            matrix_adder.C[i][j](C[i][j]);
        }
    }

    // Initialize matrices A and B
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            A[i][j].write(i + 1);
            B[i][j].write(i + 1);
        }
    }

    rst.write(true);
    sc_start(10, SC_NS); // Reset for 10 ns
    rst.write(false);
    start.write(true);
    sc_start(10, SC_NS); // Start for 10 ns
    start.write(false);
    sc_start(10, SC_NS); // Wait for completion

    // Print the result matrix C
    cout << "Result matrix is" << endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << C[i][j].read() << " ";
        }
        cout << endl;
    }

    return 0;
}
