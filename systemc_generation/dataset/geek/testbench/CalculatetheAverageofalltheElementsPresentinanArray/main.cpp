#include <systemc.h>
#include <cassert>
#include <iostream>

// AverageCalculator 模組：只負責計算平均值
SC_MODULE(AverageCalculator) {
    // 使用 sc_vector 來接收 9 個整數輸入
    sc_vector< sc_in<int> > data_in;
    // 輸出 port：平均值
    sc_out<double> average_out;

    // Constructor，指定 data_in 的大小為 9 個
    SC_CTOR(AverageCalculator)
    : data_in("data_in", 9)
    {
        SC_METHOD(compute_average);
        // 將每個輸入訊號設為敏感
        for (int i = 0; i < 9; i++) {
            sensitive << data_in[i];
        }
    }

    // 計算平均值的過程
    void compute_average() {
        int sum = 0;
        for (int i = 0; i < 9; i++) {
            sum += data_in[i].read();
        }
        average_out.write(static_cast<double>(sum) / 9);
    }
};

// Testbench 模組：提供輸入資料與檢查計算結果
SC_MODULE(Testbench) {
    // 建立 9 個 sc_signal<int>，用於傳遞資料
    sc_vector< sc_signal<int> > data_sig;
    // 信號用來接收平均值結果
    sc_signal<double> avg_sig;
    // AverageCalculator 模組實例
    AverageCalculator avg_calc_inst;

    // Constructor，初始化各項模組與連接訊號
    SC_CTOR(Testbench)
    : data_sig("data_sig", 9),   // 建立 9 個資料訊號
      avg_calc_inst("avg_calc_inst")
    {
        // 將 Testbench 的資料訊號連接到 AverageCalculator 的輸入
        for (int i = 0; i < 9; i++) {
            avg_calc_inst.data_in[i](data_sig[i]);
        }
        // 連接平均值輸出訊號
        avg_calc_inst.average_out(avg_sig);

        // 啟動測試程序
        SC_THREAD(run_test);
    }

    // 測試程序：提供資料、等待計算結果、並進行斷言檢查
    void run_test() {
        // 提供測試資料
        int test_data[9] = {10, 2, 3, 4, 5, 6, 7, 8, 9};
        for (int i = 0; i < 9; i++) {
            data_sig[i].write(test_data[i]);
        }
        // 等待一個模擬時間單位以確保計算完成
        wait(1, SC_NS);

        // 讀取並檢查結果
        double computed_avg = avg_sig.read();
        std::cout << "Computed average: " << computed_avg << std::endl;
        assert(computed_avg == 6.0);

        // 提供另一組測試資料
        int test_data2[9] = {1, 2, 3, 4, 5, 6, 7, 8, 10};
        for (int i = 0; i < 9; i++) {
            data_sig[i].write(test_data2[i]);
        }
        // 等待計算完成
        wait(1, SC_NS);

        // 讀取並檢查結果
        computed_avg = avg_sig.read();
        std::cout << "Computed average: " << computed_avg << std::endl;
        double epsilon = 1e-5; // 允許的誤差範圍
        assert(fabs(computed_avg - 5.11111) < epsilon); // 檢查計算結果是否在允許範圍內

        std::cout << "Test passed successfully." << std::endl;

        // 結束模
        sc_stop();
    }
};

// sc_main：最上層模組，啟動測試
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
