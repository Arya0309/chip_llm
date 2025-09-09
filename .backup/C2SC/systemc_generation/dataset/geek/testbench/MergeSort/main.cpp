#include <systemc.h>

// ─────────────── MergeSort Module (DUT) ───────────────
SC_MODULE(MergeSort) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    static const int N = 6; // Array size
    sc_in<int>  in_arr[N];
    sc_out<int> out_arr[N];

    int buf[N];
    int leftBuf[N];
    int rightBuf[N];

    SC_CTOR(MergeSort) {
        SC_THREAD(sort_proc);
        sensitive << clk.pos();
    }

    void merge(int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        for (int i = 0; i < n1; i++)
            leftBuf[i] = buf[left + i];
        for (int j = 0; j < n2; j++)
            rightBuf[j] = buf[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (leftBuf[i] <= rightBuf[j])
                buf[k++] = leftBuf[i++];
            else
                buf[k++] = rightBuf[j++];
        }

        while (i < n1) buf[k++] = leftBuf[i++];
        while (j < n2) buf[k++] = rightBuf[j++];
    }

    void mergeSort(int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(left, mid);
            mergeSort(mid + 1, right);
            merge(left, mid, right);
        }
    }

    void sort_proc() {
        while (true) {
            wait();
            if (start.read()) {
                for (int i = 0; i < N; ++i)
                    buf[i] = in_arr[i].read();

                mergeSort(0, N - 1);

                for (int i = 0; i < N; ++i)
                    out_arr[i].write(buf[i]);

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
    sc_signal<int> sig_in[6], sig_out[6];

    MergeSort* dut;

    SC_CTOR(Testbench) {
        dut = new MergeSort("MergeSort");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 6; ++i) {
            dut->in_arr[i](sig_in[i]);
            dut->out_arr[i](sig_out[i]);
        }

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int data[6]     = {12, 11, 13, 5, 6, 7};
        int expected[6] = {5, 6, 7, 11, 12, 13};

        for (int i = 0; i < 6; ++i)
            sig_in[i].write(data[i]);

        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        wait(done.posedge_event());

        for (int i = 0; i < 6; ++i) {
            int v = sig_out[i].read();
            std::cout << "Element " << i << " sorted value: " << v << '\n';
            assert(v == expected[i]);
        }
        std::cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

// ─────────────── sc_main ───────────────
int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
