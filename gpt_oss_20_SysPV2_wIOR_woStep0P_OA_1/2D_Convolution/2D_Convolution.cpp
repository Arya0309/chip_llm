/* === IO REQUIREMENT (HINT) ===
Expected input consists of:
    input data: 4×4 matrix 
    kernel: 3×3 matrix
Expected output consists of:
    output data: 4×4 matrix
*/

constexpr int IN_H = 4;
constexpr int IN_W = 4;
constexpr int KER_H = 3;
constexpr int KER_W = 3;
constexpr int PAD_H = KER_H / 2;
constexpr int PAD_W = KER_W / 2;
constexpr int PADDED_H = IN_H + 2 * PAD_H;   // 6
constexpr int PADDED_W = IN_W + 2 * PAD_W;   // 6
constexpr int OUT_H = IN_H;                 // 4
constexpr int OUT_W = IN_W;                 // 4

void conv2D(
    const double input[IN_H][IN_W],
    const double kernel[KER_H][KER_W],
    double output[OUT_H][OUT_W])
{
    double padded[PADDED_H][PADDED_W] = {};  // zero‑initialized

    // Copy input into padded array
    for (int i = 0; i < IN_H; ++i) {
        for (int j = 0; j < IN_W; ++j) {
            padded[i + PAD_H][j + PAD_W] = input[i][j];
        }
    }

    // Convolution
    for (int i = 0; i < OUT_H; ++i) {
        for (int j = 0; j < OUT_W; ++j) {
            double sum = 0.0;
            for (int m = 0; m < KER_H; ++m) {
                for (int n = 0; n < KER_W; ++n) {
                    sum += padded[i + m][j + n] * kernel[m][n];
                }
            }
            output[i][j] = sum;
        }
    }
}