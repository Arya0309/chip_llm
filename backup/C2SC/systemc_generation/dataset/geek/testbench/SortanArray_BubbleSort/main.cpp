#include <systemc.h>

// ─────────────── BubbleSort (DUT) ───────────────
SC_MODULE(BubbleSort) {
    sc_in<bool>  clk;           // 時脈
    sc_in<bool>  start;         // 觸發排序
    sc_out<bool> done;          // 完成旗標

    sc_in<int>   in_arr[4];     // 4-element 輸入總線
    sc_out<int>  out_arr[4];    // 4-element 輸出總線

    int buf[4];                 // 內部緩衝

    SC_CTOR(BubbleSort) {
        SC_THREAD(sort_proc);
        sensitive << clk.pos();
    }

    void sort_proc() {
        const int N = 4;
        while (true) {
            wait();                   // posedge clk
            if (start.read()) {
                /* 1. 讀入資料 */
                for (int i = 0; i < N; ++i)
                    buf[i] = in_arr[i].read();

                /* 2. Bubble sort */
                for (int i = 0; i < N - 1; ++i)
                    for (int j = 0; j < N - i - 1; ++j)
                        if (buf[j] > buf[j + 1])
                            std::swap(buf[j], buf[j + 1]);

                /* 3. 寫回結果 */
                for (int i = 0; i < N; ++i)
                    out_arr[i].write(buf[i]);

                /* 4. done 拉高一個時脈週期 */
                done.write(true);
                wait();
                done.write(false);
            }
        }
    }
};

// ──────────────── Testbench ────────────────
SC_MODULE(Testbench) {
    sc_clock        clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int>  sig_in[4], sig_out[4];

    BubbleSort*     dut;

    SC_CTOR(Testbench) {
        /* 例化並連線 */
        dut = new BubbleSort("BubbleSort");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 4; ++i) {
            dut->in_arr[i](sig_in[i]);
            dut->out_arr[i](sig_out[i]);
        }

        SC_THREAD(run_tests);   // thread 於模擬開始執行
    }

    void run_tests() {
        int data[4]     = {6, 0, 3, 5};
        int expected[4] = {0, 3, 5, 6};

        /* ▸ 餵入資料 */
        for (int i = 0; i < 4; ++i)
            sig_in[i].write(data[i]);

        /* ▸ 觸發排序 */
        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        /* ▸ 等待 done=1 */
        wait(done.posedge_event());

        /* ▸ 讀出結果並驗證 */
        for (int i = 0; i < 4; ++i) {
            int v = sig_out[i].read();
            std::cout << "Element " << i << " sorted value: " << v << '\n';
            assert(v == expected[i]);
        }
        std::cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

// ──────────────── sc_main ────────────────
int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
