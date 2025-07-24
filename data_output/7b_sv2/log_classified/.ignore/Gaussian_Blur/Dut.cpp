
#include "Dut.h"

const int WIDTH = 64;
const int HEIGHT = 64;
const int KERNEL_SIZE = 5;
const int KERNEL_RADIUS = KERNEL_SIZE / 2;

static const int KERNEL[KERNEL_SIZE][KERNEL_SIZE] = {
    { 1,  4,  7,  4, 1},
    { 4, 16, 26, 16, 4},
    { 7, 26, 41, 26, 7},
    { 4, 16, 26, 16, 4},
    { 1,  4,  7,  4, 1}
};

const int FACTOR = 273;

void gaussianBlur(const float* input, float* output, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float sum = 0.0f;
            for (int ky = -KERNEL_RADIUS; ky <= KERNEL_RADIUS; ++ky) {
                for (int kx = -KERNEL_RADIUS; kx <= KERNEL_RADIUS; ++kx) {
                    int px = x + kx;
                    if (px < 0) px = 0;
                    else if (px >= width) px = width - 1;
                    int py = y + ky;
                    if (py < 0) py = 0;
                    else if (py >= height) py = height - 1;

                    int weight = KERNEL[ky + KERNEL_RADIUS][kx + KERNEL_RADIUS];
                    sum += input[py * width + px] * weight;
                }
            }
            output[y * width + x] = sum / FACTOR;
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
        float input[WIDTH * HEIGHT];
        float output[WIDTH * HEIGHT];
        // read input matrix element-by-element
        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            input[i] = i_input.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        gaussianBlur(input, output, WIDTH, HEIGHT);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write output matrix back out
        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            o_output.write(output[i]);
        }
        /* === Variable Section End === */
    }
}
