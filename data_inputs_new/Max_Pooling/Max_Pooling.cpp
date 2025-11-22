#include <iostream>
#include <fstream>
using namespace std;

void maxPooling(const int* input,
                int in_h, int in_w,
                int pool_h, int pool_w,
                int stride_h, int stride_w,
                int* output,
                int& out_h, int& out_w) {
    out_h = (in_h - pool_h) / stride_h + 1;
    out_w = (in_w - pool_w) / stride_w + 1;

    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            int max_val = input[(i * stride_h) * in_w + (j * stride_w)];
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
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    const int in_h = 4, in_w = 4;
    const int input_size = in_h * in_w; // 16
    int input[input_size];
    
    // 設定一個足夠大的 output buffer (最大就是 4x4 = 16)
    int output[16]; 
    int pool_h, pool_w, stride_h, stride_w;
    int out_h, out_w;

    int count = 0;

    // 2. 迴圈讀取 100 筆資料
    // 每一筆測資有 20 個數字 (16 input + 4 params)
    while (count < 100 && infile >> input[0]) {
        
        // 讀取 input 矩陣剩下的 15 個數
        for (int i = 1; i < input_size; ++i) {
            infile >> input[i];
        }

        // 讀取參數：pool_h, pool_w, stride_h, stride_w
        infile >> pool_h >> pool_w >> stride_h >> stride_w;

        // 執行 Max Pooling
        maxPooling(input, in_h, in_w,
                   pool_h, pool_w,
                   stride_h, stride_w,
                   output, out_h, out_w);

        // 3. 輸出結果到 ans.txt
        // 計算總輸出元素個數
        int total_out = out_h * out_w;
        for (int i = 0; i < total_out; ++i) {
            outfile << output[i] << (i == total_out - 1 ? "" : " ");
        }
        outfile << endl;

        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
