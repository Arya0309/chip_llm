#include <iostream>
#include <cmath>
using namespace std;

// 計算長度為 length 的 input 陣列的 Softmax，結果寫入 output 陣列
void softmax(const double* input, double* output, int length) {
    // 找最大值以提升數值穩定性
    double max_val = input[0];
    for (int i = 1; i < length; ++i) {
        if (input[i] > max_val) {
            max_val = input[i];
        }
    }

    // 計算 exp(x - max_val) 並累加總和
    double sum = 0.0;
    for (int i = 0; i < length; ++i) {
        output[i] = exp(input[i] - max_val);
        sum += output[i];
    }

    // 正規化
    for (int i = 0; i < length; ++i) {
        output[i] /= sum;
    }
}

int main() {
    const int N = 5;
    double input[N]  = { 1.0, 2.0, 3.0, 2.0, 1.0 };
    double output[N];

    softmax(input, output, N);

    cout << "Softmax result: ";
    for (int i = 0; i < N; ++i) {
        cout << output[i] << " ";
    }
    cout << endl;

    return 0;
}
