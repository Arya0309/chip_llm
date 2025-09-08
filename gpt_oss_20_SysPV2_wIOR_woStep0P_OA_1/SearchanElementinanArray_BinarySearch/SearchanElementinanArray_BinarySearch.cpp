// Synthesizable binary search implementation
// No I/O streams are used; the function can be mapped to hardware.

#include <cstddef>  // For size_t

// Binary search on a statically sized array
bool binarySearch(const int* arr, int size, int target) {
    int low = 0;
    int high = size - 1;

    while (low <= high) {
        int mid = (high - low) / 2 + low;

        if (arr[mid] == target) {
            return true;
        }

        if (arr[mid] > target) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return false;
}

// Simple testbench (simulation only; no I/O)
int main() {
    const int N = 6;
    int A[N] = {1, 3, 5, 7, 9, 11};
    int query = 7;

    bool found = binarySearch(A, N, query);

    // The result can be checked in a simulation environment.
    // Returning 0 if found, 1 otherwise (just for illustration).
    return found ? 0 : 1;
}