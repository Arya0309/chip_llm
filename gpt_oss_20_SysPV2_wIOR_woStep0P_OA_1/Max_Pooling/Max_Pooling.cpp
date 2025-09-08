// Synthesizable max‑pooling using compile‑time parameters
constexpr int MAX_H = 4;
constexpr int MAX_W = 4;

// Template parameters define pooling window size and stride
template<int POOL_H, int POOL_W, int STRIDE_H, int STRIDE_W>
void maxPooling(const int input[MAX_H][MAX_W],
                int in_h, int in_w,
                int output[MAX_H][MAX_W],
                int& out_h, int& out_w)
{
    // Clamp input dimensions to array bounds
    if (in_h > MAX_H) in_h = MAX_H;
    if (in_w > MAX_W) in_w = MAX_W;

    // Compute output dimensions (guaranteed to be <= MAX_H/MAX_W)
    out_h = (in_h - POOL_H) / STRIDE_H + 1;
    out_w = (in_w - POOL_W) / STRIDE_W + 1;

    // Ensure output dimensions do not exceed array bounds
    if (out_h > MAX_H) out_h = MAX_H;
    if (out_w > MAX_W) out_w = MAX_W;
    if (out_h < 0) out_h = 0;
    if (out_w < 0) out_w = 0;

    // Iterate over all possible output positions (fixed bounds)
    for (int i = 0; i < MAX_H; ++i) {
        for (int j = 0; j < MAX_W; ++j) {
            // Skip positions outside the computed output size
            if (i >= out_h || j >= out_w) {
                continue;
            }

            // Initialize maximum with the first element in the pooling window
            int max_val = input[i * STRIDE_H][j * STRIDE_W];

            // Scan the pooling window (compile‑time bounds)
            for (int m = 0; m < POOL_H; ++m) {
                for (int n = 0; n < POOL_W; ++n) {
                    int idx_h = i * STRIDE_H + m;
                    int idx_w = j * STRIDE_W + n;
                    // Bounds are guaranteed by the clamped input dimensions
                    if (idx_h < in_h && idx_w < in_w) {
                        if (input[idx_h][idx_w] > max_val) {
                            max_val = input[idx_h][idx_w];
                        }
                    }
                }
            }

            // Store the result
            output[i][j] = max_val;
        }
    }
}