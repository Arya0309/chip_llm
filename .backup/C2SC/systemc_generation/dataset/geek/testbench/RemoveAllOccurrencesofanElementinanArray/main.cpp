#include <systemc.h>

// Module to remove all occurrences of a target value from an array
SC_MODULE(RemoveAllOccurrences) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    sc_in<int>  in_arr[11];
    sc_out<int> out_arr[11]; // output array will have 11 or fewer elements
    sc_in<int>  target;

    int buf[11]; // internal buffer

    SC_CTOR(RemoveAllOccurrences) {
        SC_THREAD(remove_proc);
        sensitive << clk.pos();
    }

    void remove_proc() {
        while (true) {
            wait();
            if (start.read()) {
                int t = target.read();
                int ind = 0;
                // Copy elements that are not equal to target
                for (int i = 0; i < 11; ++i) {
                    int val = in_arr[i].read();
                    if (val != t) {
                        buf[ind++] = val;
                    }
                }
                // Write result to output
                for (int i = 0; i < ind; ++i) {
                    out_arr[i].write(buf[i]);
                }
                // Fill rest with -1 (or any sentinel)
                for (int i = ind; i < 11; ++i) {
                    out_arr[i].write(-1);
                }
                done.write(true);
                wait();
                done.write(false);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int> in_arr[11], out_arr[11];
    sc_signal<int> target;

    RemoveAllOccurrences* dut;

    SC_CTOR(Testbench) {
        dut = new RemoveAllOccurrences("RemoveAllOccurrences");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        dut->target(target);
        for (int i = 0; i < 11; ++i) {
            dut->in_arr[i](in_arr[i]);
            dut->out_arr[i](out_arr[i]);
        }

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int data[11] = {1, 4, 3, 6, 8, 3, 9, 10, 3, 3, 7};
        int t = 3;
        int expected[11] = {1, 4, 6, 8, 9, 10, 7, -1, -1, -1, -1};

        for (int i = 0; i < 11; ++i)
            in_arr[i].write(data[i]);
        target.write(t);

        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        wait(done.posedge_event());

        for (int i = 0; i < 7; ++i) {
            int val = out_arr[i].read();
            cout << "out_arr[" << i << "]: " << val << endl;
            assert(val == expected[i]);
        }

        cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
