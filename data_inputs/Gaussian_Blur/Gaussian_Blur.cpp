#include <iostream>
using namespace std;

// 對 input (寬度為 width，高度為 height) 做 5×5 Gaussian blur，結果寫入 output
void gaussianBlur(const float* input, float* output, int width, int height) {
    const int kernelSize = 5;
    const int kernelRadius = kernelSize / 2;
    // 固定權重，總和為 273
    static const int kernel[5][5] = {
        { 1,  4,  7,  4, 1},
        { 4, 16, 26, 16, 4},
        { 7, 26, 41, 26, 7},
        { 4, 16, 26, 16, 4},
        { 1,  4,  7,  4, 1}
    };
    const int factor = 273;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float sum = 0.0f;
            // 捲積
            for (int ky = -kernelRadius; ky <= kernelRadius; ++ky) {
                for (int kx = -kernelRadius; kx <= kernelRadius; ++kx) {
                    int px = x + kx;
                    if (px < 0) px = 0;
                    else if (px >= width) px = width - 1;
                    int py = y + ky;
                    if (py < 0) py = 0;
                    else if (py >= height) py = height - 1;

                    int weight = kernel[ky + kernelRadius][kx + kernelRadius];
                    sum += input[py * width + px] * weight;
                }
            }
            output[y * width + x] = sum / factor;
        }
    }
}

int main() {
    // 範例圖片尺寸
    int width  = 1920;
    int height = 1080;

    // 動態配置緩衝區
    float* input  = new float[width * height];
    float* output = new float[width * height];

    // 初始化輸入（範例：用像素索引模 256）
    for (int i = 0; i < width * height; ++i) {
        input[i] = static_cast<float>(i % 256) / 255.0f;
    }

    // 執行 Gaussian blur
    gaussianBlur(input, output, width, height);

    // 印出第一個像素值做簡單驗證
    cout << "Blurred pixel [0]: " << output[0] << endl;

    delete[] input;
    delete[] output;
    return 0;
}
