#include <iostream>
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
    int input[ROWS][COLS] = {
        {10, 20, 30, 40, 50, 60},
        {15, 25, 35, 45, 55, 65},
        {20, 30, 40, 50, 60, 70},
        {25, 35, 45, 55, 65, 75},
        {30, 40, 50, 60, 70, 80}
    };
    int output[ROWS][COLS];
    int window_size = 3;

    median_filter(input, output, ROWS, COLS, window_size);

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cout << output[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}
