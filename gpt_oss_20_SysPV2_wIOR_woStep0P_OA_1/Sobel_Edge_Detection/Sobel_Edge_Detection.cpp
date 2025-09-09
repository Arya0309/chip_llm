#include <cstdint>
#include <cstdlib>   // for std::abs

// Compile‑time constants for the image dimensions
const int WIDTH  = 64;
const int HEIGHT = 64;

// Sobel edge detection – synthesizable implementation
void sobelEdgeDetection(const uint8_t* input, uint8_t* output)
{
    // Sobel kernels
    const int Gx[3][3] = {
        { -1,  0,  1 },
        { -2,  0,  2 },
        { -1,  0,  1 }
    };
    const int Gy[3][3] = {
        { -1, -2, -1 },
        {  0,  0,  0 },
        {  1,  2,  1 }
    };

    // Zero the border pixels
    for (int x = 0; x < WIDTH; ++x) {
        output[x] = 0;
        output[(HEIGHT - 1) * WIDTH + x] = 0;
    }
    for (int y = 0; y < HEIGHT; ++y) {
        output[y * WIDTH] = 0;
        output[y * WIDTH + (WIDTH - 1)] = 0;
    }

    // Convolution over the interior pixels
    for (int y = 1; y < HEIGHT - 1; ++y) {
        for (int x = 1; x < WIDTH - 1; ++x) {
            int sumX = 0;
            int sumY = 0;

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int pixel = input[(y + ky) * WIDTH + (x + kx)];
                    sumX += Gx[ky + 1][kx + 1] * pixel;
                    sumY += Gy[ky + 1][kx + 1] * pixel;
                }
            }

            int magnitude = std::abs(sumX) + std::abs(sumY);
            if (magnitude > 255) magnitude = 255;
            output[y * WIDTH + x] = static_cast<uint8_t>(magnitude);
        }
    }
}