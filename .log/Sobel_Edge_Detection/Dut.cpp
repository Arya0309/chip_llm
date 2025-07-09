
#include "Dut.h"

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
        const unsigned char* input = i_input.read().data;
        unsigned char* output = i_output.write().data;
        int width = i_width.read();
        int height = i_height.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        sobelEdgeDetection(input, output, width, height);
        /* === Main function Section End === */

        /* === Variable Section === */
        // No need to write anything back as output is written directly in sobelEdgeDetection
        /* === Variable Section End === */
    }
}
