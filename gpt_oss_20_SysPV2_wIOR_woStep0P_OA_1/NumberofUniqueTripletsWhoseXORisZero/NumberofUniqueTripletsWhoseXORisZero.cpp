#include <cstddef>

constexpr int MAX_N = 256; // maximum supported array size

int countTriplets(int a[], int n) 
{
    int count = 0;
    for (int i = 0; i < MAX_N; ++i) {
        if (i >= n) break;
        for (int j = i + 1; j < MAX_N; ++j) {
            if (j >= n) break;
            int xr = a[i] ^ a[j];
            for (int k = 0; k < MAX_N; ++k) {
                if (k >= n) break;
                if (k != i && k != j && a[k] == xr) {
                    count++;
                    break; // found a matching element, no need to continue
                }
            }
        }
    }
    return count / 3;
}