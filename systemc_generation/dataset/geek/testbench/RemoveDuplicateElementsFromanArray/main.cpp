#include <systemc.h>

// ─────────────── RemoveDuplicates (DUT) ───────────────
SC_MODULE(RemoveDuplicates) {
    sc_in<bool> clk;             // Clock
    sc_in<bool> start;           // Start signal
    sc_out<bool> done;           // Done signal

    sc_in<int>   in_arr[9];      // Input array (max 9 elements)
    sc_out<int>  out_arr[9];     // Output array (duplicates removed)
    sc_out<int>  out_size;       // Output new size

    int temp[9];                 // Temporary storage

    SC_CTOR(RemoveDuplicates) {
        SC_THREAD(remove_proc);
        sensitive << clk.pos();
    }

    void remove_proc() {
        const int N = 9;
        while (true) {
            wait();
            if (start.read()) {
                int buf[9];
                for (int i = 0; i < N; ++i)
                    buf[i] = in_arr[i].read();

                int j = 0;
                for (int i = 0; i < N - 1; ++i)
                    if (buf[i] != buf[i + 1])
                        temp[j++] = buf[i];
                temp[j++] = buf[N - 1];

                for (int i = 0; i < j; ++i)
                    out_arr[i].write(temp[i]);

                out_size.write(j);

                done.write(true);
                wait();
                done.write(false);
            }
        }
    }
};

// ──────────────── Testbench ────────────────
SC_MODULE(Testbench) {
    sc_clock clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int> sig_in[9], sig_out[9];
    sc_signal<int> sig_out_size;

    RemoveDuplicates* dut;

    SC_CTOR(Testbench) {
        dut = new RemoveDuplicates("RemoveDuplicates");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 9; ++i) {
            dut->in_arr[i](sig_in[i]);
            dut->out_arr[i](sig_out[i]);
        }
        dut->out_size(sig_out_size);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int data[9] = {1, 2, 2, 3, 4, 4, 4, 5, 5};
        int expected[5] = {1, 2, 3, 4, 5};

        for (int i = 0; i < 9; ++i)
            sig_in[i].write(data[i]);

        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        wait(done.posedge_event());

        int new_size = sig_out_size.read();
        cout << "New size: " << new_size << endl;

        for (int i = 0; i < new_size; ++i) {
            int v = sig_out[i].read();
            cout << "Element " << i << ": " << v << endl;
            assert(v == expected[i]);
        }

        cout << "All tests passed successfully." << endl;
        sc_stop();
    }
};

// ──────────────── sc_main ────────────────
int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
