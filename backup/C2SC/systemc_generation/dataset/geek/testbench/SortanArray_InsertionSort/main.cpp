#include <systemc.h>

// ─────────────── RowWiseSorter ───────────────
SC_MODULE(RowWiseSorter) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    sc_in<int>  in_mat[4][4];  // 4x4 input matrix
    sc_out<int> out_mat[4][4]; // 4x4 output matrix

    int buf[4][4];

    SC_CTOR(RowWiseSorter) {
        SC_THREAD(process);
        sensitive << clk.pos();
    }

    void process() {
        const int ROWS = 4, COLS = 4;
        while (true) {
            wait();
            if (start.read()) {
                // Step 1: Read input
                for (int i = 0; i < ROWS; ++i)
                    for (int j = 0; j < COLS; ++j)
                        buf[i][j] = in_mat[i][j].read();

                // Step 2: Sort each row using bubble sort
                for (int i = 0; i < ROWS; ++i) {
                    for (int j = 0; j < COLS; ++j) {
                        for (int k = 0; k < COLS - j - 1; ++k) {
                            if (buf[i][k] > buf[i][k + 1])
                                std::swap(buf[i][k], buf[i][k + 1]);
                        }
                    }
                }

                // Step 3: Write sorted output
                for (int i = 0; i < ROWS; ++i)
                    for (int j = 0; j < COLS; ++j)
                        out_mat[i][j].write(buf[i][j]);

                // Step 4: Signal done
                done.write(true);
                wait();
                done.write(false);
            }
        }
    }
};

// ─────────────── Testbench ───────────────
SC_MODULE(Testbench) {
    sc_clock clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int> sig_in[4][4], sig_out[4][4];

    RowWiseSorter* dut;

    SC_CTOR(Testbench) {
        dut = new RowWiseSorter("RowWiseSorter");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                dut->in_mat[i][j](sig_in[i][j]);
                dut->out_mat[i][j](sig_out[i][j]);
            }

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int data[4][4] = {
            {9, 8, 7, 1},
            {7, 3, 0, 2},
            {9, 5, 3, 2},
            {6, 3, 1, 2}
        };

        int expected[4][4] = {
            {1, 7, 8, 9},
            {0, 2, 3, 7},
            {2, 3, 5, 9},
            {1, 2, 3, 6}
        };

        // Write input
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                sig_in[i][j].write(data[i][j]);

        // Trigger sort
        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        // Wait for done signal
        wait(done.posedge_event());

        // Check result
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                int val = sig_out[i][j].read();
                std::cout << "out[" << i << "][" << j << "] = " << val << std::endl;
                assert(val == expected[i][j]);
            }

        cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

// ─────────────── sc_main ───────────────
int sc_main(int, char*[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}