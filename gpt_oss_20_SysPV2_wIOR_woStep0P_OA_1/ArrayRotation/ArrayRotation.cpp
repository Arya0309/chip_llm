#include <cstddef>  // for size_t

// Rotate a 6‑element array left by d positions
void Rotate(int arr[6], int d)
{
    // Normalize d to be within [0,5]
    d = d % 6;
    if (d < 0) d += 6;   // handle negative d
    if (d == 0) return;

    int temp[6];
    int k = 0;

    // Copy elements from index d to end
    for (int i = d; i < 6; ++i) {
        temp[k++] = arr[i];
    }

    // Copy the first d elements
    for (int i = 0; i < d; ++i) {
        temp[k++] = arr[i];
    }

    // Copy back to original array
    for (int i = 0; i < 6; ++i) {
        arr[i] = temp[i];
    }
}

// Example usage (no I/O, just a placeholder function)
// The function could be called from a testbench or higher‑level module.
void Example()
{
    int data[6] = {1, 2, 3, 4, 5, 6};
    Rotate(data, 2);  // data becomes {3,4,5,6,1,2}
}