#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

void softmax(const double* input, double* output, int length) {
    double max_val = input[0];
    for (int i = 1; i < length; ++i) {
        if (input[i] > max_val) {
            max_val = input[i];
        }
    }

    double sum = 0.0;
    for (int i = 0; i < length; ++i) {
        output[i] = exp(input[i] - max_val);
        sum += output[i];
    }

    for (int i = 0; i < length; ++i) {
        output[i] /= sum;
    }
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");
    const int N = 5;

    double input[N];
    double output[N];
    int count = 0;

    // 2. 迴圈讀取 100 筆資料
    // 每次讀取 5 個浮點數
    while (count < 100 && infile >> input[0]) {
        
        // 讀取剩下的 4 個數值
        for (int i = 1; i < N; ++i) {
            infile >> input[i];
        }

        // 執行 Softmax
        softmax(input, output, N);

        // 3. 輸出結果到 ans.txt
        // 將 5 個結果數值印在同一行
        for (int i = 0; i < N; ++i) {
            outfile << output[i] << (i == N - 1 ? "" : " ");
        }
        outfile << endl;

        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
