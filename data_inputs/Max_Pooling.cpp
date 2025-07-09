#include <iostream>
using namespace std;

// input: pointer to 平面化輸入陣列（row-major）
// in_h, in_w: 輸入高度、寬度
// pool_h, pool_w: pooling 視窗高度、寬度
// stride_h, stride_w: 步長
// output: 指向平面化輸出陣列的記憶體（需由呼叫端配置足夠空間）
// out_h, out_w: 執行後回傳的輸出高度、寬度
void maxPooling(const int* input,
                int in_h, int in_w,
                int pool_h, int pool_w,
                int stride_h, int stride_w,
                int* output,
                int& out_h, int& out_w) {
    // 計算輸出維度
    out_h = (in_h - pool_h) / stride_h + 1;
    out_w = (in_w - pool_w) / stride_w + 1;

    // 對每個輸出位置執行 pooling
    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            int max_val = input[(i * stride_h) * in_w + (j * stride_w)];
            // 在 pooling 視窗內找最大值
            for (int m = 0; m < pool_h; ++m) {
                for (int n = 0; n < pool_w; ++n) {
                    int idx = (i * stride_h + m) * in_w + (j * stride_w + n);
                    if (input[idx] > max_val) {
                        max_val = input[idx];
                    }
                }
            }
            output[i * out_w + j] = max_val;
        }
    }
}

int main() {
    // 範例：4×4 輸入，2×2 視窗，步長 2
    const int in_h = 4, in_w = 4;
    const int pool_h = 2, pool_w = 2;
    const int stride_h = 2, stride_w = 2;

    int input[in_h * in_w] = {
         1,  2,  3,  4,
         5,  6,  7,  8,
         9, 10, 11, 12,
        13, 14, 15, 16
    };

    int out_h, out_w;
    // 分配輸出記憶體
    int output[((in_h - pool_h) / stride_h + 1) *
               ((in_w - pool_w) / stride_w + 1)];

    maxPooling(input, in_h, in_w,
               pool_h, pool_w,
               stride_h, stride_w,
               output, out_h, out_w);

    // 印出結果
    cout << "Output (" << out_h << "x" << out_w << "):\n";
    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            cout << output[i * out_w + j] << " ";
        }
        cout << endl;
    }
    return 0;
}
