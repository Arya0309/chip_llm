#include <iostream>
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
    int width  = 64;
    int height = 64;

    unsigned char* input  = new unsigned char[width * height];
    unsigned char* output = new unsigned char[width * height];

    for(int i = 0; i < width * height; i++){
        input[i] = static_cast<unsigned char>(i % 256);
    }

    sobelEdgeDetection(input, output, width, height);

    for(int i = 0; i < 1000; i++){
        cout << static_cast<int>(output[i]) << " ";
    }
    cout << endl;

    delete[] input;
    delete[] output;
    return 0;
}
