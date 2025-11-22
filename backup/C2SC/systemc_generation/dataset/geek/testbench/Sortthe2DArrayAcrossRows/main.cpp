#include <systemc.h>

//--------------------------------------
//  Row-wise sorter ─ 無 helper、穩固握手
//--------------------------------------
SC_MODULE(RowWiseSorter)
{
    sc_in<bool>  clk;
    sc_in<bool>  start;
    sc_out<bool> done;

    sc_in <int> data_in [16];
    sc_out<int> data_out[16];

    int mat[4][4];

    SC_CTOR(RowWiseSorter)
    {
        SC_THREAD(sort_process);
        sensitive << clk.pos();
    }

    void sort_process()
    {
        while (true) {
            wait();                            // 等下一個 posedge
            if (start.read()) {
                // 載入
                for (int i = 0; i < 4; ++i)
                    for (int j = 0; j < 4; ++j)
                        mat[i][j] = data_in[i*4 + j].read();

                rowWiseSort();                 // 排序

                // 輸出
                for (int i = 0; i < 4; ++i)
                    for (int j = 0; j < 4; ++j)
                        data_out[i*4 + j].write(mat[i][j]);

                done.write(true);              // 告知完成

                // *** FIX ①：等 start 被拉低，再清掉 done
                do { wait(); } while (start.read());
                done.write(false);
            }
        }
    }

    void rowWiseSort()
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                for (int k = 0; k < 4 - j - 1; ++k)
                    if (mat[i][k] > mat[i][k + 1])
                        std::swap(mat[i][k], mat[i][k + 1]);
    }
};

//--------------------------------------
//  Testbench – 只用訊號 + 事件等待
//--------------------------------------
SC_MODULE(Testbench)
{
    sc_clock        clk;
    sc_signal<bool> start, done;

    sc_signal<int> din [16];
    sc_signal<int> dout[16];

    RowWiseSorter* sorter;

    SC_CTOR(Testbench)
        : clk("clk", 1, SC_NS)
    {
        sorter = new RowWiseSorter("RowWiseSorter");
        sorter->clk(clk);
        sorter->start(start);
        sorter->done(done);
        for (int i = 0; i < 16; ++i) {
            sorter->data_in [i](din [i]);
            sorter->data_out[i](dout[i]);
        }

        SC_THREAD(run_tests);
    }

    void run_tests()
    {
        int matrix_in[4][4] = {
            {9, 8, 7, 1},
            {7, 3, 0, 2},
            {9, 5, 3, 2},
            {6, 3, 1, 2}
        };
        int expected[4][4] = {
            {1, 7, 8, 9},
            {0, 2, 3, 7},
            {2, 3, 5, 9},
            {1, 2, 3, 6}
        };

        // 輸入資料
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                din[i*4 + j].write(matrix_in[i][j]);

        // 觸發排序
        start.write(true);

        // *** FIX ②：直接等待 done 的上升沿，不會 miss
        wait(done.posedge_event());

        start.write(false);  // 告知 sorter 已接收完成訊號

        // 讀回並驗證
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                int val = dout[i*4 + j].read();
                assert(val == expected[i][j]);
                std::cout << "Row " << i << ", Col " << j
                          << " sorted value: " << val << '\n';
            }

        std::cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

//--------------------------------------
//  sc_main
//--------------------------------------
int sc_main(int argc, char* argv[])
{
    Testbench tb("tb");
    sc_start();
    return 0;
}
