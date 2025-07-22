#include <iostream>
using namespace std;

void gaussianBlur(const float* input, float* output, int width, int height) {
    const int kernelSize = 5;
    const int kernelRadius = kernelSize / 2;
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
    int width  = 64;
    int height = 64;
    float* input  = new float[width * height];
    float* output = new float[width * height];

    for (int i = 0; i < width * height; ++i) {
        input[i] = static_cast<float>(i % 256) / 255.0f;
    }

    gaussianBlur(input, output, width, height);

    cout << "Blurred pixel [0]: " << output[0] << endl;

    delete[] input;
    delete[] output;
    return 0;
}
