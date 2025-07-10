#include <iostream>
using namespace std;

// 使用原始陣列實作線性 SVM（Hinge Loss + L2 正規化，採用 SGD）
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

int main() {
    // 範例資料：4 筆樣本，每筆 2 維特徵
    const int n = 4, d = 2;
    double X[n * d] = {
        3.0, 3.0,
        4.0, 3.0,
        1.0, 1.0,
        0.0, 2.0
    };
    int y[n] = { 1, 1, -1, -1 };

    double w[d];
    double b;
    double C = 1.0;
    double lr = 0.001;
    int epochs = 1000;

    // 訓練
    trainSVM(X, y, n, d, C, lr, epochs, w, b);

    // 輸出結果
    cout << "Weights: ";
    for (int j = 0; j < d; j++)
        cout << w[j] << " ";
    cout << "\nBias: " << b << endl;

    return 0;
}
