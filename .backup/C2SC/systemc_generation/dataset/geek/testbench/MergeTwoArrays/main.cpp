#include <systemc.h>
#include <algorithm>

// ─────────────── MergeArrays (DUT) ───────────────
SC_MODULE(MergeArrays) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    sc_in<int>  arr1[4];
    sc_in<int>  arr2[4];
    sc_out<int> arr3[8];

    int buf1[4], buf2[4], buf3[8];

    SC_CTOR(MergeArrays) {
        SC_THREAD(merge_proc);
        sensitive << clk.pos();
    }

    void merge_proc() {
        while (true) {
            wait();
            if (start.read()) {
                // 1. 讀入兩個陣列
                for (int i = 0; i < 4; ++i) buf1[i] = arr1[i].read();
                for (int i = 0; i < 4; ++i) buf2[i] = arr2[i].read();

                // 2. 合併陣列
                int k = 0;
                for (int i = 0; i < 4; ++i) buf3[k++] = buf1[i];
                for (int i = 0; i < 4; ++i) buf3[k++] = buf2[i];

                // 3. 排序
                std::sort(buf3, buf3 + 8);

                // 4. 寫回輸出
                for (int i = 0; i < 8; ++i)
                    arr3[i].write(buf3[i]);

                // 5. 拉高 done 一個 cycle
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
    sc_signal<int> sig_arr1[4], sig_arr2[4], sig_arr3[8];

    MergeArrays* dut;

    SC_CTOR(Testbench) {
        dut = new MergeArrays("MergeArrays");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        for (int i = 0; i < 4; ++i) dut->arr1[i](sig_arr1[i]);
        for (int i = 0; i < 4; ++i) dut->arr2[i](sig_arr2[i]);
        for (int i = 0; i < 8; ++i) dut->arr3[i](sig_arr3[i]);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int data1[4] = {1, 3, 5, 7};
        int data2[4] = {2, 4, 6, 8};
        int expected[8] = {1, 2, 3, 4, 5, 6, 7, 8};

        for (int i = 0; i < 4; ++i) sig_arr1[i].write(data1[i]);
        for (int i = 0; i < 4; ++i) sig_arr2[i].write(data2[i]);

        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        wait(done.posedge_event());

        for (int i = 0; i < 8; ++i) {
            int val = sig_arr3[i].read();
            std::cout << "arr3[" << i << "]: " << val << std::endl;
            assert(val == expected[i]);
        }

        std::cout << "All merge tests passed successfully." << std::endl;
        sc_stop();
    }
};

// ─────────────── sc_main ───────────────
int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}