
#include "Dut.h"

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
        const float* input = i_input.read().data;
        float* output = i_output.write().data;
        int width = i_width.read();
        int height = i_height.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        gaussianBlur(input, output, width, height);
        /* === Main function Section End === */

        /* === Variable Section === */
        // No need to write anything back as output is written directly in gaussianBlur
        /* === Variable Section End === */
    }
}
