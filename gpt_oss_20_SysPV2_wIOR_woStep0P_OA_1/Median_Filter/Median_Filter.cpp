#define ROWS 5
#define COLS 6
#define WINDOW_SIZE 3   // Must be odd

void median_filter(const int input[ROWS][COLS], int output[ROWS][COLS]) {
    const int half = WINDOW_SIZE / 2;
    const int max_window = ROWS * COLS;
    int window[max_window];

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            int count = 0;

            // Collect window elements using static offset loops
            for (int di = -half; di <= half; ++di) {
                int wi = i + di;
                if (wi < 0 || wi >= ROWS) continue;
                for (int dj = -half; dj <= half; ++dj) {
                    int wj = j + dj;
                    if (wj < 0 || wj >= COLS) continue;
                    window[count++] = input[wi][wj];
                }
            }

            // Bubble sort over the collected elements (static bounds)
            for (int x = 0; x < max_window - 1; ++x) {
                for (int y = x + 1; y < max_window; ++y) {
                    if (x < count && y < count && window[x] > window[y]) {
                        int tmp = window[x];
                        window[x] = window[y];
                        window[y] = tmp;
                    }
                }
            }

            // Median is at position count/2
            output[i][j] = window[count / 2];
        }
    }
}