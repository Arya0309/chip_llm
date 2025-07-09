
#include "Dut.h"

void mlp_feedforward(
    const float input[INPUT_SIZE],
    float hidden[HIDDEN_SIZE],
    float output[OUTPUT_SIZE],
    const float w1[HIDDEN_SIZE][INPUT_SIZE],
    const float b1[HIDDEN_SIZE],
    const float w2[OUTPUT_SIZE][HIDDEN_SIZE],
    const float b2[OUTPUT_SIZE]
) {
    // 第一層：Input → Hidden (含偏差與 ReLU)
    for (int i = 0; i < HIDDEN_SIZE; ++i) {
        float sum = b1[i];
        for (int j = 0; j < INPUT_SIZE; ++j) {
            sum += w1[i][j] * input[j];
        }
        hidden[i] = (sum > 0.0f) ? sum : 0.0f;  // ReLU
    }
    // 第二層：Hidden → Output (線性輸出)
    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        float sum = b2[i];
        for (int j = 0; j < HIDDEN_SIZE; ++j) {
            sum += w2[i][j] * hidden[j];
        }
        output[i] = sum;
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
        float input[INPUT_SIZE];
        for (int i = 0; i < INPUT_SIZE; ++i) {
            input[i] = i_input[i].read();
        }
        float hidden[HIDDEN_SIZE];
        float output[OUTPUT_SIZE];
        const float w1[HIDDEN_SIZE][INPUT_SIZE] = {{...}};  // Initialize weights
        const float b1[HIDDEN_SIZE] = {...};  // Initialize biases
        const float w2[OUTPUT_SIZE][HIDDEN_SIZE] = {{...}};  // Initialize weights
        const float b2[OUTPUT_SIZE] = {...};  // Initialize biases
        /* === Variable Section End === */

        /* === Main function Section === */
        mlp_feedforward(input, hidden, output, w1, b1, w2, b2);
        /* === Main function Section End === */

        /* === Variable Section === */
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            o_output[i].write(output[i]);
        }
        /* === Variable Section End === */
    }
}
