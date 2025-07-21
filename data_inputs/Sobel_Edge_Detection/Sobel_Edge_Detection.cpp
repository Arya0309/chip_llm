#include <iostream>
using namespace std;

// Sobel Edge Detection 函式
// input:  原始灰階影像 (寬度 width、高度 height)
// output: Sobel 邊緣強度影像 (同上)
void sobelEdgeDetection(const unsigned char* input,
                        unsigned char* output,
                        int width,
                        int height)
{
    int Gx[3][3] = {
        { -1,  0,  1 },
        { -2,  0,  2 },
        { -1,  0,  1 }
    };
    int Gy[3][3] = {
        { -1, -2, -1 },
        {  0,  0,  0 },
        {  1,  2,  1 }
    };

    // 先將邊界設為 0
    for(int x = 0; x < width; x++){
        output[x] = 0;
        output[(height-1)*width + x] = 0;
    }
    for(int y = 0; y < height; y++){
        output[y*width] = 0;
        output[y*width + (width-1)] = 0;
    }

    // 主迴圈：對每個非邊界像素套用 Sobel 核
    for(int y = 1; y < height - 1; y++){
        for(int x = 1; x < width - 1; x++){
            int sumX = 0;
            int sumY = 0;
            for(int ky = -1; ky <= 1; ky++){
                for(int kx = -1; kx <= 1; kx++){
                    int pixel = input[(y + ky) * width + (x + kx)];
                    sumX += Gx[ky+1][kx+1] * pixel;
                    sumY += Gy[ky+1][kx+1] * pixel;
                }
            }
            int magnitude = abs(sumX) + abs(sumY);
            if(magnitude > 255) magnitude = 255;
            output[y*width + x] = static_cast<unsigned char>(magnitude);
        }
    }
}

int main()
{
    // 範例影像大小
    int width  = 640;
    int height = 480;

    // 建立輸入/輸出緩衝
    unsigned char* input  = new unsigned char[width * height];
    unsigned char* output = new unsigned char[width * height];

    // TODO: 在此填入或載入實際影像資料
    for(int i = 0; i < width * height; i++){
        input[i] = static_cast<unsigned char>(i % 256); // 模擬灰階影像
    }

    // 執行 Sobel 邊緣檢測
    sobelEdgeDetection(input, output, width, height);

    // 範例輸出：將前 1000 個像素值印到螢幕
    for(int i = 0; i < 1000; i++){
        cout << static_cast<int>(output[i]) << " ";
    }
    cout << endl;

    delete[] input;
    delete[] output;
    return 0;
}
