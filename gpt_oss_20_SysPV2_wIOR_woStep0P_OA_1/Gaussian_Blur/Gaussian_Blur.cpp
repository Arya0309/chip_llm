/* === IO REQUIREMENT (HINT) ===
Expected input consists of:
    a 64×64 grayscale image matrix of float
Expected output consists of:
    a 64×64 grayscale image matrix of float (blurred)
*/

constexpr int WIDTH  = 64;
constexpr int HEIGHT = 64;

void gaussianBlur(const float* input, float* output) {
    constexpr int kernelSize  = 5;
    constexpr int kernelRadius = kernelSize / 2;
    static constexpr int kernel[5][5] = {
        { 1,  4,  7,  4, 1},
        { 4, 16, 26, 16, 4},
        { 7, 26, 41, 26, 7},
        { 4, 16, 26, 16, 4},
        { 1,  4,  7,  4, 1}
    };
    constexpr int factor = 273;

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            float sum = 0.0f;

            for (int ky = -kernelRadius; ky <= kernelRadius; ++ky) {
                for (int kx = -kernelRadius; kx <= kernelRadius; ++kx) {
                    int px = x + kx;
                    if (px < 0) px = 0;
                    else if (px >= WIDTH) px = WIDTH - 1;
                    int py = y + ky;
                    if (py < 0) py = 0;
                    else if (py >= HEIGHT) py = HEIGHT - 1;

                    int weight = kernel[ky + kernelRadius][kx + kernelRadius];
                    sum += input[py * WIDTH + px] * weight;
                }
            }
            output[y * WIDTH + x] = sum / factor;
        }
    }
}