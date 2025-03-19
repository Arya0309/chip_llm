
#include <systemc.h>

SC_MODULE(MatrixOperations) {
    sc_in_clk clk;
    sc_out<int> trace_out;
    sc_out<double> normal_out;

    SC_CTOR(MatrixOperations) {
        SC_METHOD(calculate_trace_normal);
        sensitive << clk.pos();
    }

    void calculate_trace_normal() {
        const int MAX = 5;
        int mat[MAX][MAX] = {{1, 1, 1, 1, 1},
                             {2, 2, 2, 2, 2},
                             {3, 3, 3, 3, 3},
                             {4, 4, 4, 4, 4},
                             {5, 5, 5, 5, 5}};
        int trace = 0;
        double normal = 0;

        // Calculate trace
        for (int i = 0; i < MAX; i++)
            trace += mat[i][i];

        // Calculate normal (Frobenius norm)
        double sum = 0;
        for (int i = 0; i < MAX; i++)
            for (int j = 0; j < MAX; j++)
                sum += mat[i][j] * mat[i][j];
        normal = sqrt(sum);

        // Output results
        trace_out.write(trace);
        normal_out.write(normal);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clk", 10, SC_NS);
    sc_signal<int> trace_result;
    sc_signal<double> normal_result;

    MatrixOperations matrix_ops("matrix_ops");
    matrix_ops.clk(clock);
    matrix_ops.trace_out(trace_result);
    matrix_ops.normal_out(normal_result);

    sc_start(10, SC_NS);

    std::cout << "Trace of Matrix = " << trace_result.read() << std::endl;
    std::cout << "Normal of Matrix = " << normal_result.read() << std::endl;

    return 0;
}
