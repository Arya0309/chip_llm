#include <iostream>
using namespace std;

const int INPUT_SIZE  = 128;
const int HIDDEN_SIZE = 64;
const int OUTPUT_SIZE = 10;

// MLP Feedforward 函式
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

int main() {
    // 範例輸入
    float input[INPUT_SIZE];
    for (int i = 0; i < INPUT_SIZE; ++i) {
        input[i] = i * 0.01f;
    }

    // 輸出與中間層暫存
    float hidden[HIDDEN_SIZE];
    float output[OUTPUT_SIZE];

    // 權重與偏差宣告並初始化
    static float w1[HIDDEN_SIZE][INPUT_SIZE];
    static float b1[HIDDEN_SIZE];
    static float w2[OUTPUT_SIZE][HIDDEN_SIZE];
    static float b2[OUTPUT_SIZE];

    // 以簡單模式初始化權重與偏差（可替換為實際數值）
    for (int i = 0; i < HIDDEN_SIZE; ++i) {
        b1[i] = 0.1f * i;
        for (int j = 0; j < INPUT_SIZE; ++j) {
            w1[i][j] = 0.01f * (i + j);
        }
    }
    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        b2[i] = -0.1f * i;
        for (int j = 0; j < HIDDEN_SIZE; ++j) {
            w2[i][j] = 0.01f * (i - j);
        }
    }

    // 執行前向傳播
    mlp_feedforward(input, hidden, output, w1, b1, w2, b2);

    // 列印輸出結果
    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        cout << "Output[" << i << "] = " << output[i] << endl;
    }

    return 0;
}
