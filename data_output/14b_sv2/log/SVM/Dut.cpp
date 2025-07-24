
#include "Dut.h"

constexpr int MAX_N = 4;
constexpr int MAX_D = 2;

void trainSVM(double* X, int* y,
              int n,            // 样本数
              int d,            // 特征维度
              double C,         // 正则化参数
              double lr,        // 学习率
              int epochs,       // 迭代次数
              double* w,        // 输出：权重向量长度 d
              double& b) {      // 输出：偏差
    // 初始化
    for (int j = 0; j < d; j++) w[j] = 0.0;
    b = 0.0;

    // SGD 循环
    for (int it = 0; it < epochs; it++) {
        for (int i = 0; i < n; i++) {
            // 计算 dot(w, x_i)
            double dot = 0.0;
            for (int j = 0; j < d; j++)
                dot += w[j] * X[i * d + j];
            double margin = y[i] * (dot + b);

            if (margin < 1) {
                // 亚梯度：dw = w - C * y_i * x_i, db = -C * y_i
                for (int j = 0; j < d; j++) {
                    w[j] += lr * (C * y[i] * X[i * d + j] - w[j]);
                }
                b += lr * C * y[i];
            } else {
                // 只做正则化梯度
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
        double X[MAX_N * MAX_D];
        int y[MAX_N];
        int n = i_n.read();
        int d = i_d.read();
        double C = i_C.read();
        double lr = i_lr.read();
        int epochs = i_epochs.read();
        double w[MAX_D];
        double b;
        /* === Variable Section End === */

        // Read input matrices and vectors
        for (int i = 0; i < n * d; ++i) {
            X[i] = i_X.read();
        }
        for (int i = 0; i < n; ++i) {
            y[i] = i_y.read();
        }

        /* === Main function Section === */
        trainSVM(X, y, n, d, C, lr, epochs, w, b);
        /* === Main function Section End === */

        // Write output weights and bias
        for (int j = 0; j < d; ++j) {
            o_w.write(w[j]);
        }
        o_b.write(b);
    }
}
