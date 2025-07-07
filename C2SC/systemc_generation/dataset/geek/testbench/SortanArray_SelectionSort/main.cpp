#include <systemc.h>

//─────────────────── DUT ───────────────────
SC_MODULE(SelectionSort) {
    sc_in<bool>  clk;
    sc_in<bool>  start;
    sc_out<bool> done;

    sc_in<int>   in_arr[5];   // 輸入資料
    sc_out<int>  out_arr[5];  // 排序結果

    int buf[5];               // 內部緩衝

    SC_CTOR(SelectionSort) {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
    }

    void sort_process() {
        const int N = 5;
        while (true) {
            wait();                           // posedge clk
            if (start.read()) {
                /* 1. 讀入資料 */
                for (int i = 0; i < N; ++i)
                    buf[i] = in_arr[i].read();

                /* 2. selection sort */
                for (int i = 0; i < N - 1; ++i) {
                    int min_idx = i;
                    for (int j = i + 1; j < N; ++j)
                        if (buf[j] < buf[min_idx]) min_idx = j;
                    std::swap(buf[i], buf[min_idx]);
                }

                /* 3. 寫回結果 */
                for (int i = 0; i < N; ++i)
                    out_arr[i].write(buf[i]);

                /* 4. 拉高 done 一個週期 */
                done.write(true);
                wait();
                done.write(false);
            }
        }
    }
};

//─────────────────── Testbench ───────────────────
SC_MODULE(Testbench) {
    sc_clock        clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int>  sig_in[5], sig_out[5];

    SelectionSort*  dut;

    SC_CTOR(Testbench) {
        dut = new SelectionSort("SelectionSort");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 5; ++i) {
            dut->in_arr[i](sig_in[i]);
            dut->out_arr[i](sig_out[i]);
        }

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int data[5]     = {64, 25, 12, 22, 11};
        int expected[5] = {11, 12, 22, 25, 64};

        /*  驅動輸入  */
        for (int i = 0; i < 5; ++i)
            sig_in[i].write(data[i]);

        /*  觸發排序  */
        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        /*  等待 done  */
        wait(done.posedge_event());

        /*  讀取結果並驗證  */
        for (int i = 0; i < 5; ++i) {
            int v = sig_out[i].read();
            std::cout << "Element " << i << " sorted value: " << v << '\n';
            assert(v == expected[i]);
        }
        std::cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

//─────────────────── sc_main ───────────────────
int sc_main(int argc, char* argv[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
