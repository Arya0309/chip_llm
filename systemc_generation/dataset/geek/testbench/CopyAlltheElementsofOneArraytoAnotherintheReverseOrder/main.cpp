#include <systemc.h>

// ──────────────────── ArrayReverse ────────────────────
SC_MODULE(ArrayReverse) {
    sc_in<bool>  clk;
    sc_in<bool>  start;
    sc_out<bool> done;

    sc_in<int>   in_arr[5];   // 5-element input bus
    sc_out<int>  out_arr[5];  // 5-element output bus

    int buf[5];               // internal buffer

    SC_CTOR(ArrayReverse) {
        SC_THREAD(reverse_process);
        sensitive << clk.pos();
    }

    void reverse_process() {
        while (true) {
            wait();                      // posedge clk
            if (start.read()) {
                /* 1. 讀入資料 */
                for (int i = 0; i < 5; ++i)
                    buf[i] = in_arr[i].read();

                /* 2. 反轉 */
                for (int i = 0; i < 5; ++i)
                    out_arr[i].write(buf[4 - i]);  // 直接寫回輸出埠

                /* 3. 完成訊號 */
                done.write(true);
                wait();                // 再延一個週期
                done.write(false);
            }
        }
    }
};

// ───────────────────── Testbench ─────────────────────
SC_MODULE(Testbench) {
    sc_clock        clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int>  sig_in[5], sig_out[5];

    ArrayReverse*   dut;

    SC_CTOR(Testbench) {
        dut = new ArrayReverse("ArrayReverse");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 5; ++i) {
            dut->in_arr[i](sig_in[i]);
            dut->out_arr[i](sig_out[i]);
        }

        SC_THREAD(run_tests);   // 無需 static sensitivity
    }

    void run_tests() {
        int original[5] = {1, 2, 3, 4, 5};
        int expected[5] = {5, 4, 3, 2, 1};

        /* ▸ 驅動輸入 */
        for (int i = 0; i < 5; ++i)
            sig_in[i].write(original[i]);

        /* ▸ 觸發反轉 */
        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        /* ▸ 等待 done=1 */
        wait(done.posedge_event());

        /* ▸ 讀取結果並驗證 */
        std::cout << "Reversed Array: ";
        for (int i = 0; i < 5; ++i) {
            int val = sig_out[i].read();
            std::cout << val << " ";
            assert(val == expected[i]);
        }
        std::cout << "\nAll tests passed successfully.\n";

        sc_stop();
    }
};

// ───────────────────── sc_main ─────────────────────
int sc_main(int argc, char* argv[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
