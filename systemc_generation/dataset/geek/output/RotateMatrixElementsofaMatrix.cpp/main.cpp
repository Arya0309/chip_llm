
#include <systemc.h>

SC_MODULE(MatrixRotator) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_out<sc_uint<32>> out;

    // Internal storage for the matrix
    sc_uint<32> mat[4][4];

    // Control signals
    sc_signal<int> row, col, m, n;
    sc_signal<sc_uint<32>> prev, curr;
    sc_signal<bool> done;

    SC_CTOR(MatrixRotator) {
        SC_METHOD(rotate_matrix);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(print_matrix);
        sensitive << done;
    }

    void rotate_matrix() {
        if (reset.read()) {
            // Initialize matrix with test values
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    mat[i][j] = (i * 4) + j + 1;
                }
            }
            row.write(0);
            col.write(0);
            m.write(4);
            n.write(4);
            done.write(false);
        } else {
            if (!done.read()) {
                if (row.read() < m.read() && col.read() < n.read()) {
                    if (row.read() + 1 == m.read() || col.read() + 1 == n.read()) {
                        done.write(true);
                    } else {
                        prev.write(mat[row.read() + 1][col.read()]);
                        for (int i = col.read(); i < n.read(); i++) {
                            curr.write(mat[row.read()][i]);
                            mat[row.read()][i] = prev.read();
                            prev.write(curr.read());
                        }
                        row.write(row.read() + 1);

                        for (int i = row.read(); i < m.read(); i++) {
                            curr.write(mat[i][n.read() - 1]);
                            mat[i][n.read() - 1] = prev.read();
                            prev.write(curr.read());
                        }
                        n.write(n.read() - 1);

                        if (row.read() < m.read()) {
                            for (int i = n.read() - 1; i >= col.read(); i--) {
                                curr.write(mat[m.read() - 1][i]);
                                mat[m.read() - 1][i] = prev.read();
                                prev.write(curr.read());
                            }
                        }
                        m.write(m.read() - 1);

                        if (col.read() < n.read()) {
                            for (int i = m.read() - 1; i >= row.read(); i--) {
                                curr.write(mat[i][col.read()]);
                                mat[i][col.read()] = prev.read();
                                prev.write(curr.read());
                            }
                        }
                        col.write(col.read() + 1);
                    }
                }
            }
        }
    }

    void print_matrix() {
        if (done.read()) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    std::cout << mat[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }
    }
};

SC_MODULE(Testbench) {
    sc_out<bool> clk;
    sc_out<bool> reset;

    SC_CTOR(Testbench) {
        SC_THREAD(generate_clk);
        SC_THREAD(generate_reset);
    }

    void generate_clk() {
        clk.write(false);
        wait(10, SC_NS);
        while (true) {
            clk.write(!clk.read());
            wait(10, SC_NS);
        }
    }

    void generate_reset() {
        reset.write(true);
        wait(20, SC_NS);
        reset.write(false);
        wait(100, SC_NS);
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    MatrixRotator matrix_rotator("matrix_rotator");
    Testbench tb("tb");

    sc_clock clk("clk", 10, SC_NS, 0.5, 0, SC_NS, false);
    sc_signal<bool> reset;

    matrix_rotator.clk(clk);
    matrix_rotator.reset(reset);
    tb.clk(clk);
    tb.reset(reset);

    sc_start();
    return 0;
}
