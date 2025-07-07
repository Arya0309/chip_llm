#include <systemc.h>
#include <assert.h>
#include <iostream>

//─────────────── MinMax (DUT) ───────────────
SC_MODULE(MinMax) {
    sc_in<bool>  clk;
    sc_in<bool>  start;
    sc_out<bool> done;
    sc_out<int>  min_val;
    sc_out<int>  max_val;

    sc_in<int>   in_arr[5];   // 5-element input bus

    SC_CTOR(MinMax) {
        SC_THREAD(compute_process);
        sensitive << clk.pos();
    }

    void compute_process() {
        const int N = 5;
        while (true) {
            wait();                       // posedge clk
            if (start.read()) {
                /* 1. 讀入並初始化 */
                int local_min = in_arr[0].read();
                int local_max = local_min;

                /* 2. 掃描其餘元素 */
                for (int i = 1; i < N; ++i) {
                    int v = in_arr[i].read();
                    if (v < local_min) local_min = v;
                    if (v > local_max) local_max = v;
                }

                /* 3. 輸出結果 */
                min_val.write(local_min);
                max_val.write(local_max);

                /* 4. done 拉高一個週期 */
                done.write(true);
                wait();
                done.write(false);
            }
        }
    }
};

//─────────────── Testbench ───────────────
SC_MODULE(Testbench) {
    sc_clock        clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int>  min_val, max_val;
    sc_signal<int>  sig_in[5];

    MinMax*         dut;

    SC_CTOR(Testbench) {
        /* 建立 DUT 並連線 */
        dut = new MinMax("MinMax");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        dut->min_val(min_val);
        dut->max_val(max_val);
        for (int i = 0; i < 5; ++i)
            dut->in_arr[i](sig_in[i]);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int data[5] = {12, 1234, 45, 67, 1};

        /* ▸ 驅動輸入 */
        for (int i = 0; i < 5; ++i)
            sig_in[i].write(data[i]);

        /* ▸ 觸發運算 */
        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        /* ▸ 等待 done=1 */
        wait(done.posedge_event());

        /* ▸ 讀取並驗證結果 */
        int computed_min = min_val.read();
        int computed_max = max_val.read();

        assert(computed_min == 1);
        assert(computed_max == 1234);

        std::cout << "Computed minimum: " << computed_min << '\n';
        std::cout << "Computed maximum: " << computed_max << '\n';
        std::cout << "Test passed successfully.\n";

        sc_stop();
    }
};

//─────────────── sc_main ───────────────
int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
