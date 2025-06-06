// matrix_add.cpp
#include <systemc.h>

#define MATRIX_SIZE 32

using namespace sc_core;

// ===== DUT 模組 =====
SC_MODULE(Dut) {
    sc_in_clk        i_clk;
    sc_in<bool>      i_rst;       // (目前沒用到 reset，但留著以備擴充)
    sc_fifo_in<int>  i_a;
    sc_fifo_in<int>  i_b;
    sc_fifo_out<int> o_result;

    SC_CTOR(Dut) {
        SC_THREAD(compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void compute() {
        while (true) {
            int x = i_a.read();
            int y = i_b.read();
            o_result.write(x + y);
            wait();  // 等下一個時鐘
        }
    }
};

// ===== Testbench 模組 =====
SC_MODULE(Testbench) {
    sc_in_clk        i_clk;
    sc_out<bool>     o_rst;
    sc_fifo_out<int> o_a;
    sc_fifo_out<int> o_b;
    sc_fifo_in<int>  i_result;

    int A[MATRIX_SIZE][MATRIX_SIZE];
    int B[MATRIX_SIZE][MATRIX_SIZE];

    SC_CTOR(Testbench) {
        // 初始化 32×32 矩陣：前 4×4 如題，其他設為 0
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                if (i < 4 && j < 4) {
                    A[i][j] = i + 1;
                    B[i][j] = i + 1;
                } else {
                    A[i][j] = 0;
                    B[i][j] = 0;
                }
            }
        }
        SC_THREAD(test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void test() {
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                // 送資料
                o_a.write(A[i][j]);
                o_b.write(B[i][j]);
                wait();  // 等 DUT 處理

                // 讀回結果並檢查
                int res = i_result.read();
                int exp = (i < 4 && j < 4) ? 2 * (i + 1) : 0;
                if (res != exp) {
                    std::cerr << "Error at (" << i << "," << j << "): got "
                              << res << ", expected " << exp << std::endl;
                    sc_stop();
                    return;
                }
            }
        }
        std::cout << "All matrix addition tests passed successfully." << std::endl;
        sc_stop();
    }
};

// ===== sc_main =====
int sc_main(int argc, char* argv[]) {
    // 時鐘與 reset（目前不驅動 reset）
    sc_clock        clk("clk", 1, SC_NS);
    sc_signal<bool> rst("rst");

    // FIFO channel（預設深度 16）
    sc_fifo<int> fifo_a;
    sc_fifo<int> fifo_b;
    sc_fifo<int> fifo_res;

    // 實例化 DUT 與 Testbench
    Dut        dut("dut");
    Testbench tb("tb");

    // 連接時鐘與 reset
    dut.i_clk(clk);
    dut.i_rst(rst);
    tb.i_clk(clk);
    tb.o_rst(rst);

    // 連接 FIFO
    dut.i_a(fifo_a);
    dut.i_b(fifo_b);
    dut.o_result(fifo_res);
    tb.o_a(fifo_a);
    tb.o_b(fifo_b);
    tb.i_result(fifo_res);

    // 啟動模擬
    sc_start();
    return 0;
}
