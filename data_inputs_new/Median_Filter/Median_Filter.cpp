#include <iostream>
#include <fstream>
using namespace std;

#define ROWS 5
#define COLS 6

void median_filter(const int input[ROWS][COLS], int output[ROWS][COLS], int rows, int cols, int window_size) {
    int half = window_size / 2;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int count = 0;
            int* window = new int[window_size * window_size];
            for (int wi = i - half; wi <= i + half; ++wi) {
                for (int wj = j - half; wj <= j + half; ++wj) {
                    if (wi >= 0 && wi < rows && wj >= 0 && wj < cols) {
                        window[count++] = input[wi][wj];
                    }
                }
            }
            for (int x = 0; x < count - 1; ++x) {
                for (int y = x + 1; y < count; ++y) {
                    if (window[x] > window[y]) {
                        int tmp = window[x];
                        window[x] = window[y];
                        window[y] = tmp;
                    }
                }
            }
            output[i][j] = window[count / 2];
            delete[] window;
        }
    }
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    int input[ROWS][COLS];
    int output[ROWS][COLS];
    int window_size;
    int count = 0;
    while (count < 100 && infile >> window_size) {
        
        // 讀取 5x6 矩陣 (共 30 個數)
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                infile >> input[i][j];
            }
        }

        // 執行中值濾波
        median_filter(input, output, ROWS, COLS, window_size);

        // 3. 輸出結果到 ans.txt
        // 將 5x6 結果拉平為一行，並處理尾端空白
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                // 判斷是否為最後一個元素 (Row 4, Col 5)
                bool isLast = (i == ROWS - 1) && (j == COLS - 1);
                outfile << output[i][j] << (isLast ? "" : " ");
            }
        }
        outfile << endl;

        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
