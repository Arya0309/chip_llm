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
        float hidden[HIDDEN_SIZE];
        float output[OUTPUT_SIZE];
        float w1[HIDDEN_SIZE][INPUT_SIZE];
        float b1[HIDDEN_SIZE];
        float w2[OUTPUT_SIZE][HIDDEN_SIZE];
        float b2[OUTPUT_SIZE];

        // Read inputs from FIFOs
        for (int i = 0; i < INPUT_SIZE; ++i) {
            input[i] = i_input.read();
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            b1[i] = i_b1.read();
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            b2[i] = i_b2.read();
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            for (int j = 0; j < INPUT_SIZE; ++j) {
                w1[i][j] = i_w1.read();
            }
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            for (int j = 0; j < HIDDEN_SIZE; ++j) {
                w2[i][j] = i_w2.read();
            }
        }

        // Call the function
        mlp_feedforward(input, hidden, output, w1, b1, w2, b2);

        // Write outputs to FIFOs
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            o_hidden.write(hidden[i]);
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            o_output.write(output[i]);
        }
        /* === Variable Section End === */
    }
}
