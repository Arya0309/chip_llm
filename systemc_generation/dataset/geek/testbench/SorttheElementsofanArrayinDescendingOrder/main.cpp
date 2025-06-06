#include <systemc.h>

// ──────────────── BubbleSort (DUT) ────────────────
SC_MODULE(BubbleSort) {
    sc_in<bool>  clk;          // 時脈
    sc_in<bool>  start;        // 觸發排序
    sc_out<bool> done;         // 完成旗標

    sc_in<int>   in_arr[5];    // 5-element  輸入總線
    sc_out<int>  out_arr[5];   // 5-element  輸出總線

    int buf[5];                // 內部緩衝

    SC_CTOR(BubbleSort) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
    }

    void sort_process() {
        const int N = 5;
        while (true) {
            wait();                    // 等待 posedge
            if (start.read()) {
                /* 1. 讀入資料 */
                for (int i = 0; i < N; ++i)
                    buf[i] = in_arr[i].read();

                /* 2. Bubble sort (early-exit) */
                for (int i = 0; i < N; ++i) {
                    bool swapped = false;
                    for (int j = 1; j < N - i; ++j) {
                        if (buf[j] < buf[j - 1]) {
                            std::swap(buf[j], buf[j - 1]);
                            swapped = true;
                        }
                    }
                    if (!swapped) break;
                }

                /* 3. 寫回結果 */
                for (int i = 0; i < N; ++i)
                    out_arr[i].write(buf[i]);

                /* 4. done 拉高一個週期 */
                done.write(true);
                wait();               // 下一個時脈
                done.write(false);
            }
        }
    }
};

// ───────────────── Testbench ─────────────────
SC_MODULE(Testbench) {
    sc_clock        clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int>  sig_in[5], sig_out[5];

    BubbleSort*     dut;

    SC_CTOR(Testbench) {
        dut = new BubbleSort("BubbleSort");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 5; ++i) {
            dut->in_arr[i](sig_in[i]);
            dut->out_arr[i](sig_out[i]);
        }

        SC_THREAD(run_tests);   // thread 在模擬一開始執行
    }

    void run_tests() {
        int data[5]     = {1, 12, 6, 8, 10};
        int expected[5] = {1, 6, 8, 10, 12};

        /* ▸ 驅動輸入 */
        for (int i = 0; i < 5; ++i)
            sig_in[i].write(data[i]);

        /* ▸ 觸發排序 */
        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        /* ▸ 等待 done=1 */
        wait(done.posedge_event());

        /* ▸ 讀取結果並驗證 */
        std::cout << "Sorted result: ";
        for (int i = 0; i < 5; ++i) {
            int v = sig_out[i].read();
            std::cout << v << " ";
            assert(v == expected[i]);
        }
        std::cout << "\nAll tests passed successfully.\n";

        sc_stop();
    }
};

// ──────────────── sc_main ────────────────
int sc_main(int argc, char* argv[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
