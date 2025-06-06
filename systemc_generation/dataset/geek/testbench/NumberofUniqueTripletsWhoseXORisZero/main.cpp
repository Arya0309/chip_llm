#include <systemc.h>
#include <iostream>

// ──────────────── TripletCounter Module ────────────────
SC_MODULE(TripletCounter) {
    sc_in<bool>  clk;
    sc_in<bool>  start;
    sc_out<bool> done;

    sc_in<int>  in_arr[6];
    sc_out<int> triplet_count;

    SC_CTOR(TripletCounter) {
        SC_METHOD(proc);
        sensitive << clk.pos();   // 同步到時脈正緣
        dont_initialize();        // 不在模擬 0 時刻呼叫
    }

  private:
    bool done_next = false;       // 內部暫存：下一拍要輸出的 done

    // 檢查 xr 是否存在於 buf，並排除 i、j
    bool exists(int val, const int* buf, int skip_i, int skip_j) {
        for (int k = 0; k < 6; ++k)
            if (k != skip_i && k != skip_j && buf[k] == val)
                return true;
        return false;
    }

    void proc() {
        // 先把上一拍決定好的 done 輸出去
        done.write(done_next);
        done_next = false;        // 預設下拍不送 done

        if (start.read()) {
            // Step 1: 將輸入拉到區域陣列
            int buf[6];
            for (int i = 0; i < 6; ++i)
                buf[i] = in_arr[i].read();

            // Step 2: 計算 triplet ⊕ 結果
            int count = 0;
            for (int i = 0; i < 5; ++i) {
                for (int j = i + 1; j < 6; ++j) {
                    int xr = buf[i] ^ buf[j];
                    if (xr != buf[i] && xr != buf[j] && exists(xr, buf, i, j))
                        ++count;
                }
            }
            triplet_count.write(count / 3);

            // 產生單脈波；本拍計算，下一拍拉高 done
            done_next = true;
        }
    }
};


// ──────────────── Testbench ────────────────
SC_MODULE(Testbench) {
    sc_clock clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int> sig_in[6];
    sc_signal<int> sig_out;

    TripletCounter* dut;

    SC_CTOR(Testbench) {
        dut = new TripletCounter("TripletCounter");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        dut->triplet_count(sig_out);
        for (int i = 0; i < 6; ++i)
            dut->in_arr[i](sig_in[i]);

        SC_THREAD(run_test);
    }

    void run_test() {
        int data[6] = {1, 3, 5, 10, 14, 15};

        for (int i = 0; i < 6; ++i)
            sig_in[i].write(data[i]);

        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        wait(done.posedge_event());

        int result = sig_out.read();
        std::cout << "Triplet XOR==0 count: " << result << std::endl;
        assert(result == 2); // Based on original C++ output

        std::cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}