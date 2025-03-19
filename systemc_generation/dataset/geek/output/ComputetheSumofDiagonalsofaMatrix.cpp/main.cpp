
#include <systemc.h>

SC_MODULE(DiagonalSumCalculator) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_out<int> principal_sum;
    sc_out<int> secondary_sum;

    SC_CTOR(DiagonalSumCalculator) {
        SC_METHOD(calculate_sums);
        sensitive << clk.pos();
        dont_initialize();
    }

    void calculate_sums() {
        if (reset.read()) {
            principal_sum.write(0);
            secondary_sum.write(0);
        } else {
            const int MAX = 100;
            int mat[4][MAX] = {{1, 2, 3, 4},
                              {5, 6, 7, 8},
                              {1, 2, 3, 4},
                              {5, 6, 7, 8}};
            int n = 4;
            int principal = 0, secondary = 0;

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i == j) {
                        principal += mat[i][j];
                    }
                    if ((i + j) == (n - 1)) {
                        secondary += mat[i][j];
                    }
                }
            }

            principal_sum.write(principal);
            secondary_sum.write(secondary);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS, 0.5, 0, SC_NS, true);
    sc_signal<bool> reset;
    sc_signal<int> principal_sum;
    sc_signal<int> secondary_sum;

    DiagonalSumCalculator calc("calc");
    calc.clk(clk);
    calc.reset(reset);
    calc.principal_sum(principal_sum);
    calc.secondary_sum(secondary_sum);

    reset.write(true);
    sc_start(10, SC_NS);
    reset.write(false);
    sc_start(10, SC_NS);

    cout << "Principal Diagonal: " << principal_sum.read() << endl;
    cout << "Secondary Diagonal: " << secondary_sum.read() << endl;

    return 0;
}
