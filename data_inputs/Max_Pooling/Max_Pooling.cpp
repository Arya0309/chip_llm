#include <iostream>
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
    int output[((in_h - pool_h) / stride_h + 1) *
               ((in_w - pool_w) / stride_w + 1)];

    maxPooling(input, in_h, in_w,
               pool_h, pool_w,
               stride_h, stride_w,
               output, out_h, out_w);

    cout << "Output (" << out_h << "x" << out_w << "):\n";
    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            cout << output[i * out_w + j] << " ";
        }
        cout << endl;
    }
    return 0;
}
