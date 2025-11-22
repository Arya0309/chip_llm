#include <iostream>
#include <fstream>
using namespace std;

void sobelEdgeDetection(const unsigned char* input,
                        unsigned char* output,
                        int width,
                        int height)
{
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    for(int x = 0; x < width; x++){
        output[x] = 0;
        output[(height - 1) * width + x] = 0;
    }
    for(int y = 0; y < height; y++){
        output[y * width] = 0;
        output[y * width + (width - 1)] = 0;
    }

    for(int y = 1; y < height - 1; y++){
        for(int x = 1; x < width - 1; x++){
            int sumX = 0;
            int sumY = 0;
            for(int ky = -1; ky <= 1; ky++){
                for(int kx = -1; kx <= 1; kx++){
                    int pixel = input[(y + ky) * width + (x + kx)];
                    sumX += Gx[ky + 1][kx + 1] * pixel;
                    sumY += Gy[ky + 1][kx + 1] * pixel;
                }
            }
            int magnitude = abs(sumX) + abs(sumY);
            if(magnitude > 255) magnitude = 255;
            output[y * width + x] = static_cast<unsigned char>(magnitude);
        }
    }
}

int main()
{
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");
    int width  = 64;
    int height = 64;
    int size = width * height;

    unsigned char* input  = new unsigned char[size];
    unsigned char* output = new unsigned char[size];

    int count = 0;
    int temp_val; // 用於讀取數值的暫存變數
    while (count < 100 && infile >> temp_val) {
        input[0] = static_cast<unsigned char>(temp_val);

        // 讀取剩下的 4095 個像素
        for(int i = 1; i < size; i++){
            infile >> temp_val;
            input[i] = static_cast<unsigned char>(temp_val);
        }

        // 執行 Sobel 邊緣檢測
        sobelEdgeDetection(input, output, width, height);

        // 3. 輸出結果到 ans.txt
        // 必須轉型為 int 輸出，否則會印出 ASCII 字元
        for(int i = 0; i < size; i++){
            outfile << static_cast<int>(output[i]) << (i == size - 1 ? "" : " ");
        }
        outfile << endl;

        count++;
    }

    // 釋放記憶體
    delete[] input;
    delete[] output;

    infile.close();
    outfile.close();
    return 0;
}
