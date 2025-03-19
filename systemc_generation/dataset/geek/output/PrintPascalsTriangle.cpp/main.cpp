
#include <systemc.h>

SC_MODULE(PascalsTriangle) {
    sc_in<int> n; // Input: Number of rows to print
    sc_out<sc_bv<64>> out; // Output: One row of Pascal's Triangle at a time

    SC_CTOR(PascalsTriangle) {
        SC_THREAD(printPascal);
        sensitive << n;
    }

    int binomialCoeff(int n, int k) {
        int res = 1;
        if (k > n - k)
            k = n - k;
        for (int i = 0; i < k; ++i) {
            res *= (n - i);
            res /= (i + 1);
        }
        return res;
    }

    void printPascal() {
        wait(); // Wait for n to be set
        int numRows = n.read();
        for (int line = 0; line < numRows; line++) {
            sc_bv<64> rowOutput = 0; // Assuming each row fits within 64 bits for simplicity
            int bitPos = 0;
            for (int i = 0; i <= line; i++) {
                int coeff = binomialCoeff(line, i);
                rowOutput.range(bitPos + 31, bitPos) = coeff; // Assuming each coefficient fits in 32 bits
                bitPos += 32;
            }
            out.write(rowOutput);
            wait(); // Simulate one cycle per row
        }
    }
};

int sc_main(int argc, char* argv[]) {
    PascalsTriangle pt("pt");
    sc_signal<int> num_rows;
    sc_signal<sc_bv<64>> triangle_row;

    pt.n(num_rows);
    pt.out(triangle_row);

    num_rows = 7; // Set the number of rows to print

    sc_start(10, SC_NS); // Start simulation for 10 nanoseconds

    return 0;
}
