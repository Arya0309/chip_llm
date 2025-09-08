#include <cstdint>

// Synthesizable GCD using Euclidean algorithm with a fixed iteration bound.
int gcd(uint32_t a, uint32_t b) {
    // The Euclidean algorithm will finish in at most 32 iterations
    // for 32‑bit inputs. The loop is bounded by a constant.
    for (int i = 0; i < 32; ++i) {
        if (b == 0) break;
        uint32_t t = a % b;
        a = b;
        b = t;
    }
    return static_cast<int>(a);
}

// Example top‑level function (no I/O). The host can call `gcd` directly.
int main() {
    // Placeholder: synthesis tools will treat `gcd` as the top module.
    return 0;
}