#include "Dut.h"

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

int predictSVM(double* x, double* w, double b, int d) {
    double sum = 0.0;
    for (int j = 0; j < d; j++)
        sum += w[j] * x[j];
    sum += b;
    return (sum >= 0) ? 1 : -1;
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
        double* X = i_X.read().to_ptr<double>();
        int* y = i_y.read().to_ptr<int>();
        int n = i_n.read();
        int d = i_d.read();
        double C = i_C.read();
        double lr = i_lr.read();
        int epochs = i_epochs.read();
        double* w = i_w.read().to_ptr<double>();
        double& b = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        trainSVM(X, y, n, d, C, lr, epochs, w, b);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(predictSVM(i_x.read().to_ptr<double>(), w, b, d));
        /* === Variable Section End === */
    }
}
