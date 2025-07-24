#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Testbench.h"

/* === Fixed Format === */
Testbench::Testbench(sc_module_name n)
    : sc_module(n) {
    SC_THREAD(do_feed);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(do_fetch);
    sensitive << i_clk.pos();
    dont_initialize();
}
/* === Fixed Format End === */

void Testbench::do_feed() {
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

    std::vector<float> input(WIDTH * HEIGHT);
    std::vector<float> expected(WIDTH * HEIGHT);

    // Initialize input matrix
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            input[y * WIDTH + x] = static_cast<float>(y * WIDTH + x);
        }
    }

    // Compute expected output using Gaussian blur
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            float sum = 0.0f;
            for (int ky = -KERNEL_RADIUS; ky <= KERNEL_RADIUS; ++ky) {
                for (int kx = -KERNEL_RADIUS; kx <= KERNEL_RADIUS; ++kx) {
                    int px = x + kx;
                    if (px < 0) px = 0;
                    else if (px >= WIDTH) px = WIDTH - 1;
                    int py = y + ky;
                    if (py < 0) py = 0;
                    else if (py >= HEIGHT) py = HEIGHT - 1;

                    int weight = KERNEL[ky + KERNEL_RADIUS][kx + KERNEL_RADIUS];
                    sum += input[py * WIDTH + px] * weight;
                }
            }
            expected[y * WIDTH + x] = sum / FACTOR;
        }
    }

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        o_input.write(input[i]);
    }
}

void Testbench::do_fetch() {
    const int WIDTH = 64;
    const int HEIGHT = 64;

    std::vector<float> output(WIDTH * HEIGHT);

    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        output[i] = i_output.read();
    }

    bool all_passed = true;
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        if (output[i] != expected[i]) {
            std::cerr << "Test case failed at index " << i << ": Expected " << expected[i] << ", got " << output[i] << std::endl;
            all_passed = false;
        }
    }

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }

    sc_stop();
}
