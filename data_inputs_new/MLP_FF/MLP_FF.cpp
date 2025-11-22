#include <iostream>
#include <fstream>
using namespace std;

const int INPUT_SIZE  = 32;
const int HIDDEN_SIZE = 16;
const int OUTPUT_SIZE = 10;

void mlp_feedforward(
    const float input[INPUT_SIZE],
    float hidden[HIDDEN_SIZE],
    float output[OUTPUT_SIZE],
    const float w1[HIDDEN_SIZE][INPUT_SIZE],
    const float b1[HIDDEN_SIZE],
    const float w2[OUTPUT_SIZE][HIDDEN_SIZE],
    const float b2[OUTPUT_SIZE]
) {
    for (int i = 0; i < HIDDEN_SIZE; ++i) {
        float sum = b1[i];
        for (int j = 0; j < INPUT_SIZE; ++j) {
            sum += w1[i][j] * input[j];
        }
        hidden[i] = (sum > 0.0f) ? sum : 0.0f;
    }
    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        float sum = b2[i];
        for (int j = 0; j < HIDDEN_SIZE; ++j) {
            sum += w2[i][j] * hidden[j];
        }
        output[i] = sum;
    }
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");
    float input[INPUT_SIZE];
    float hidden[HIDDEN_SIZE];
    float output[OUTPUT_SIZE];

    float w1[HIDDEN_SIZE][INPUT_SIZE];
    float b1[HIDDEN_SIZE];
    float w2[OUTPUT_SIZE][HIDDEN_SIZE];
    float b2[OUTPUT_SIZE];

    int count = 0;

    while (count < 100 && infile >> input[0]) {
        
        // (1) 讀取 Input (剩下 31 個)
        for (int i = 1; i < INPUT_SIZE; ++i) {
            infile >> input[i];
        }

        // (2) 讀取 W1 (16 * 32)
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            for (int j = 0; j < INPUT_SIZE; ++j) {
                infile >> w1[i][j];
            }
        }

        // (3) 讀取 B1 (16)
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            infile >> b1[i];
        }

        // (4) 讀取 W2 (10 * 16)
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            for (int j = 0; j < HIDDEN_SIZE; ++j) {
                infile >> w2[i][j];
            }
        }

        // (5) 讀取 B2 (10)
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            infile >> b2[i];
        }

        // 3. 執行前向傳播
        mlp_feedforward(input, hidden, output, w1, b1, w2, b2);

        // 4. 輸出結果到 ans.txt
        // 將 10 個 output 數值印在同一行
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            outfile << output[i] << (i == OUTPUT_SIZE - 1 ? "" : " ");
        }
        outfile << endl;

        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
