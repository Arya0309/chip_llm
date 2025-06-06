#include <systemc.h>

// Module to check if two arrays are equal (after sorting)
SC_MODULE(CheckArraysEqual) {
    sc_in<bool>  clk;
    sc_in<bool>  start;
    sc_out<bool> done;
    sc_out<bool> result;

    sc_in<int>   in_arr1[5];
    sc_in<int>   in_arr2[5];

    // 內部緩衝
    int buf1[5], buf2[5];

    SC_CTOR(CheckArraysEqual) {
        SC_METHOD(check_proc);
        sensitive << clk.pos();   // 每個正緣呼叫一次
        dont_initialize();        // 不在 time 0 執行
    }

    void check_proc() {
        if (start.read()) {
            // 讀取兩個陣列
            for (int i = 0; i < 5; ++i) {
                buf1[i] = in_arr1[i].read();
                buf2[i] = in_arr2[i].read();
            }

            // 排序
            std::sort(buf1, buf1 + 5);
            std::sort(buf2, buf2 + 5);

            // 比較
            bool is_equal = true;
            for (int i = 0; i < 5; ++i) {
                if (buf1[i] != buf2[i]) {
                    is_equal = false;
                    break;
                }
            }

            result.write(is_equal);
            done.write(true);     // 僅此週期拉高
        } else {
            done.write(false);    // 其他週期保持 0
        }
    }
};


// Testbench
SC_MODULE(Testbench) {
    sc_clock clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done, result;
    sc_signal<int> sig_arr1[5], sig_arr2[5];

    CheckArraysEqual* dut;

    SC_CTOR(Testbench) {
        dut = new CheckArraysEqual("CheckArraysEqual");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        dut->result(result);
        for (int i = 0; i < 5; ++i) {
            dut->in_arr1[i](sig_arr1[i]);
            dut->in_arr2[i](sig_arr2[i]);
        }

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int A[5] = {1, 2, 3, 4, 5};
        int B[5] = {5, 4, 3, 2, 1};

        for (int i = 0; i < 5; ++i) {
            sig_arr1[i].write(A[i]);
            sig_arr2[i].write(B[i]);
        }

        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        wait(done.posedge_event());

        bool res = result.read();
        std::cout << "Arrays are " << (res ? "Equal" : "Not Equal") << std::endl;
        assert(res == true);

        std::cout << "Test passed successfully.\n";
        sc_stop();
    }
};

int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}