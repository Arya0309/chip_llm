#include <systemc.h>

// Module to compute common elements of two arrays
SC_MODULE(CommonElements) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    sc_in<int> arr1[6];
    sc_in<int> arr2[6];
    sc_out<int> result[6];  // Max 6 common elements (conservative)
    sc_out<int> result_size;

    int buf1[6], buf2[6];

    SC_CTOR(CommonElements) {
        SC_THREAD(process);
        sensitive << clk.pos();
    }

    void process() {
        while (true) {
            wait();
            if (start.read()) {
                // Read inputs
                for (int i = 0; i < 6; ++i) {
                    buf1[i] = arr1[i].read();
                    buf2[i] = arr2[i].read();
                }

                // Sort inputs
                std::sort(buf1, buf1 + 6);
                std::sort(buf2, buf2 + 6);

                // Find common elements (manual intersection)
                int i = 0, j = 0, k = 0;
                while (i < 6 && j < 6) {
                    if (buf1[i] < buf2[j]) {
                        ++i;
                    } else if (buf1[i] > buf2[j]) {
                        ++j;
                    } else {
                        result[k++].write(buf1[i]);
                        ++i;
                        ++j;
                    }
                }
                for (int idx = k; idx < 6; ++idx)
                    result[idx].write(0);  // Pad with zeros
                result_size.write(k);

                // Signal completion
                done.write(true);
                wait();
                done.write(false);
            }
        }
    }
};

// Testbench
SC_MODULE(Testbench) {
    sc_clock clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int> sig_arr1[6], sig_arr2[6];
    sc_signal<int> sig_result[6], sig_result_size;

    CommonElements* dut;

    SC_CTOR(Testbench) {
        dut = new CommonElements("CommonElements");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 6; ++i) {
            dut->arr1[i](sig_arr1[i]);
            dut->arr2[i](sig_arr2[i]);
            dut->result[i](sig_result[i]);
        }
        dut->result_size(sig_result_size);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int a1[6] = {1, 45, 54, 71, 76, 12};
        int a2[6] = {1, 7, 5, 4, 6, 12};
        int expected[] = {1, 12};

        for (int i = 0; i < 6; ++i) {
            sig_arr1[i].write(a1[i]);
            sig_arr2[i].write(a2[i]);
        }

        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        wait(done.posedge_event());

        int size = sig_result_size.read();
        std::cout << "Common Elements (" << size << "): ";
        for (int i = 0; i < size; ++i) {
            int val = sig_result[i].read();
            std::cout << val << " ";
            assert(val == expected[i]);
        }
        std::cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();                // launch simulation
    return 0;
}
