/* === IO REQUIREMENT (HINT) ===
Expected input consists of:
    a 6-element array
Expected output consists of:
    a 6-element array
*/
#include <stdint.h>

#define LENGTH 6

// Reverse a fixed‑size array of LENGTH elements
void reverseArray(const int original[LENGTH], int reversed[LENGTH]) {
#pragma HLS INLINE
    for (int i = 0; i < LENGTH; i++) {
#pragma HLS PIPELINE II=1
        reversed[i] = original[LENGTH - i - 1];
    }
}