#include <iostream>
using namespace std;

// Perform a median filter on a 1D integer signal
void median_filter(const int* input, int* output, int length, int window_size) {
    int half = window_size / 2;
    for (int i = 0; i < length; ++i) {
        int count = 0;
        // Allocate window buffer
        int* window = new int[window_size];
        // Gather values in the window
        for (int j = i - half; j <= i + half; ++j) {
            if (j >= 0 && j < length) {
                window[count++] = input[j];
            }
        }
        // Simple sorting (bubble sort) of the window
        for (int x = 0; x < count - 1; ++x) {
            for (int y = x + 1; y < count; ++y) {
                if (window[x] > window[y]) {
                    int tmp = window[x];
                    window[x] = window[y];
                    window[y] = tmp;
                }
            }
        }
        // Pick median
        output[i] = window[count / 2];
        delete[] window;
    }
}

int main() {
    const int length = 15;
    int input[length] = { 5, 2, 8, 3, 7, 1, 9, 4, 6, 0, 2, 3, 1, 8, 5 };
    int output[length];
    int window_size = 3;

    median_filter(input, output, length, window_size);

    // Print filtered result
    for (int i = 0; i < length; ++i) {
        cout << output[i] << " ";
    }
    cout << endl;
    return 0;
}
