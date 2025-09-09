/* === IO REQUIREMENT (HINT) ===
Expected input consists of:
    a 6-element array
Expected output consists of:
    a 6-element array
*/
#include <stdio.h>

void swap(int* arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void bubbleSort(int arr[6]) {
    const int N = 6;
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(arr, j, j + 1);
        }
    }
}