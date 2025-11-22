#include <stdio.h>

void printArray(int arr[], int len) {
	for (int i = 0; i < len; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void reverseArray(int original[], int reversed[], int len) {
	for (int i = 0; i < len; i++) {
		reversed[i] = original[len - i - 1];
	}
}

int main() {
	int original_arr[] = {1, 2, 3, 4, 5};
	int len = sizeof(original_arr) / sizeof(original_arr[0]);
	int copied_arr[len];

	reverseArray(original_arr, copied_arr, len);

	printf("Original array: ");
	printArray(original_arr, len);

	printf("Resultant array: ");
	printArray(copied_arr, len);

	return 0;
}
