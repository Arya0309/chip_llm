// C program to copy the contents 
// of one array into another 
// in the reverse order 

#include <stdio.h> 

// Function to print the array 
void printArray(int arr[], int len) 
{ 
	int i; 
	for (i = 0; i < len; i++) { 
		printf("%d ", arr[i]); 
	} 
} 

// Driver code 
int main() 
{ 
	int original_arr[] = {1, 2, 3, 4, 5}; 
	int len = sizeof(original_arr)/sizeof(original_arr[0]); 

	int copied_arr[len], i, j; 
	
	// Copy the elements of the array 
	// in the copied_arr in Reverse Order 
	for (i = 0; i < len; i++) { 
		copied_arr[i] = original_arr[len - i - 1]; 
	} 

	// Print the original_arr 
	printf(" Original array: "); 
	printArray(original_arr, len); 

	// Print the copied array 
	printf(" Resultant array: "); 
	printArray(copied_arr, len); 

	return 0; 
} 
