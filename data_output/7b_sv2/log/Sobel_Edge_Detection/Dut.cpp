
#include "Dut.h"

#define WIDTH 64
#define HEIGHT 64

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

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        unsigned char input[WIDTH * HEIGHT];
        unsigned char output[WIDTH * HEIGHT];
        // read input matrix element-by-element
        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            input[i] = i_input.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        sobelEdgeDetection(input, output, WIDTH, HEIGHT);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result matrix back out
        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            o_output.write(output[i]);
        }
        /* === Variable Section End === */
    }
}
