#include <iostream>
#include <fstream>
using namespace std;

void trainSVM(double* X, int* y,
              int n,
              int d,
              double C,
              double lr,
              int epochs,
              double* w,
              double& b) {
    for (int j = 0; j < d; j++) w[j] = 0.0;
    b = 0.0;

    for (int it = 0; it < epochs; it++) {
        for (int i = 0; i < n; i++) {
            double dot = 0.0;
            for (int j = 0; j < d; j++)
                dot += w[j] * X[i * d + j];
            double margin = y[i] * (dot + b);

            if (margin < 1) {
                for (int j = 0; j < d; j++) {
                    w[j] += lr * (C * y[i] * X[i * d + j] - w[j]);
                }
                b += lr * C * y[i];
            } else {
                for (int j = 0; j < d; j++) {
                    w[j] += lr * (-w[j]);
                }
            }
        }
    }
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    const int n = 4, d = 2;

    // 資料緩衝區
    double X[n * d];
    int y[n];
    
    // 模型參數緩衝區
    double w[d];
    double b;
    
    // 超參數變數
    double C;
    double lr;
    int epochs;

    int count = 0;
    while (count < 100 && infile >> X[0]) {
        
        // (1) 讀取 X 的剩下 7 個數值 (共 8 個)
        for (int i = 1; i < n * d; ++i) {
            infile >> X[i];
        }

        // (2) 讀取 y (4 個整數)
        for (int i = 0; i < n; ++i) {
            infile >> y[i];
        }

        // (3) 讀取超參數 C, lr, epochs
        infile >> C >> lr >> epochs;

        // 執行 SVM 訓練
        trainSVM(X, y, n, d, C, lr, epochs, w, b);

        // 3. 輸出結果到 ans.txt
        // 格式：w[0] w[1] b (共 3 個數值)
        for (int j = 0; j < d; j++) {
            outfile << w[j] << " ";
        }
        outfile << b << endl;

        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
