
#include "Dut.h"

constexpr int N = 4;
constexpr int D = 2;

// original SVM training function
void trainSVM(double* X, int* y,
              int n,            // 樣本數
              int d,            // 特徵維度
              double C,         // 正則化參數
              double lr,        // 學習率
              int epochs,       // 迭代次數
              double* w,        // 輸出：權重向量長度 d
              double& b) {      // 輸出：偏差
    // 初始化
    for (int j = 0; j < d; j++) w[j] = 0.0;
    b = 0.0;

    // SGD 迴圈
    for (int it = 0; it < epochs; it++) {
        for (int i = 0; i < n; i++) {
            // 計算 dot(w, x_i)
            double dot = 0.0;
            for (int j = 0; j < d; j++)
                dot += w[j] * X[i * d + j];
            double margin = y[i] * (dot + b);

            if (margin < 1) {
                // 亞梯度：dw = w - C * y_i * x_i, db = -C * y_i
                for (int j = 0; j < d; j++) {
                    w[j] += lr * (C * y[i] * X[i * d + j] - w[j]);
                }
                b += lr * C * y[i];
            } else {
                // 只做正規化梯度
                for (int j = 0; j < d; j++) {
                    w[j] += lr * (-w[j]);
                }
            }
        }
    }
}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        double X[N * D];
        int y[N];
        int n;
        int d;
        double C;
        double lr;
        int epochs;
        double w[D];
        double b;

        // read inputs element-by-element
        for (int i = 0; i < N * D; ++i) {
            X[i] = i_X.read();
        }
        for (int i = 0; i < N; ++i) {
            y[i] = i_y.read();
        }
        n = i_n.read();
        d = i_d.read();
        C = i_C.read();
        lr = i_lr.read();
        epochs = i_epochs.read();
        for (int i = 0; i < D; ++i) {
            w[i] = i_w.read();
        }
        b = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        trainSVM(X, y, n, d, C, lr, epochs, w, b);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result back out
        for (int i = 0; i < D; ++i) {
            o_w.write(w[i]);
        }
        o_b.write(b);
        /* === Variable Section End === */
    }
}
