#include <cstdint>

// Synthesizable addition of two 8‑bit binary numbers.
// Returns a 9‑bit result (maximum sum of two 8‑bit numbers).
uint16_t add8bit(uint8_t a, uint8_t b) {
    // Perform addition; result fits in 9 bits.
    return static_cast<uint16_t>(a) + static_cast<uint16_t>(b);
}