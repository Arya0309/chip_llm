// Synthesizable bubble sort for a fixed-size array using templates.
// No I/O operations are present, making this code suitable for
// Cadence Stratus HLS synthesis.

#include <cstddef> // for std::size_t

// Bubble sort implementation with compile‑time size
template<std::size_t N>
void bubbleSort(int (&arr)[N]) {
    for (std::size_t i = 0; i < N - 1; ++i) {
        for (std::size_t j = 0; j < N - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

// Top‑level function that can be synthesized.
// It sorts an array of exactly 6 integers.
void sortArray(int arr[6]) {
    bubbleSort(arr);
}