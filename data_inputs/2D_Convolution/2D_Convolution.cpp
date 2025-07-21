#include <iostream>
using namespace std;

// 使用 C-style 陣列 (動態配置) 實作 2D 卷積 (帶零填充)
// input: 指向輸入資料的指標 (大小 in_h * in_w)
// kernel: 指向卷積核資料的指標 (大小 ker_h * ker_w)
// 回傳值: 新配置的輸出陣列 (大小 in_h * in_w)，呼叫者負責 delete[]

double* conv2D(
    const double* input, int in_h, int in_w,
    const double* kernel, int ker_h, int ker_w)
{
    int pad_h = ker_h / 2;
    int pad_w = ker_w / 2;
    int padded_h = in_h + 2 * pad_h;
    int padded_w = in_w + 2 * pad_w;
    int out_h = in_h;
    int out_w = in_w;

    // 動態配置並初始化為 0
    double* padded = new double[padded_h * padded_w]();
    double* output = new double[out_h * out_w];

    // 將原始輸入複製到 padded 內
    for (int i = 0; i < in_h; ++i) {
        for (int j = 0; j < in_w; ++j) {
            padded[(i + pad_h) * padded_w + (j + pad_w)] = input[i * in_w + j];
        }
    }

    // 執行卷積
    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            double sum = 0.0;
            for (int m = 0; m < ker_h; ++m) {
                for (int n = 0; n < ker_w; ++n) {
                    sum += padded[(i + m) * padded_w + (j + n)]
                         * kernel[m * ker_w + n];
                }
            }
            output[i * out_w + j] = sum;
        }
    }

    delete[] padded;
    return output;
}

int main() {
    // 範例輸入 4x4
    const int in_h = 4, in_w = 4;
    double input[in_h * in_w] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 8, 7, 6,
        5, 4, 3, 2
    };

    // 範例卷積核 3x3 平均濾波
    const int ker_h = 3, ker_w = 3;
    double kernel[ker_h * ker_w];
    for (int i = 0; i < ker_h * ker_w; ++i) {
        kernel[i] = 1.0 / 9.0;
    }

    // 執行卷積
    double* result = conv2D(input, in_h, in_w, kernel, ker_h, ker_w);

    // 輸出結果
    cout << "Result: " << endl;
    for (int i = 0; i < in_h; ++i) {
        for (int j = 0; j < in_w; ++j) {
            cout << result[i * in_w + j] << " ";
        }
        cout << endl;
    }

    delete[] result;
    return 0;
}
