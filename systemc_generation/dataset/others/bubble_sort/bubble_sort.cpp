#include <iostream>
#include <cassert>
#include <vector>

// Bubble sort function.
// Constraint hint: Array size (n) should be constrained such that 2 < n < 20 for synthesis.
void bubbleSort(std::vector<int>& arr) {
    // Enforce constraint at runtime.
    assert(arr.size() > 2 && arr.size() < 20 && "Array size must be >2 and <20 for synthesis.");
    
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {  // loop bound uses n; see constraint hint above.
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                // Swap adjacent elements.
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = true;
            }
        }
        // If no elements were swapped, the array is sorted.
        if (!swapped)
            break;
    }
}

// Helper function to check if a vector is sorted.
bool isSorted(const std::vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i - 1] > arr[i])
            return false;
    }
    return true;
}

// Testbench using assert in a void function.
void testBubbleSort() {
    {
        // Test 1: Unsorted array.
        std::vector<int> testArray = {64, 34, 25, 12, 22, 11, 90};
        bubbleSort(testArray);
        assert(isSorted(testArray) && "Test 1 failed: Array is not sorted.");
    }
    {
        // Test 2: Already sorted array.
        std::vector<int> testArray = {1, 2, 3, 4, 5};
        bubbleSort(testArray);
        assert(isSorted(testArray) && "Test 2 failed: Sorted array is not recognized as sorted.");
    }
    {
        // Test 3: Reverse sorted array.
        std::vector<int> testArray = {9, 8, 7, 6, 5};
        bubbleSort(testArray);
        assert(isSorted(testArray) && "Test 3 failed: Reverse sorted array did not sort correctly.");
    }
    {
        // Test 4: Array with duplicate values.
        std::vector<int> testArray = {3, 3, 2, 1, 2};
        bubbleSort(testArray);
        assert(isSorted(testArray) && "Test 4 failed: Array with duplicates did not sort correctly.");
    }
    // All tests passed if no assert fails.
    std::cout << "All bubble sort tests passed!" << std::endl;
}

int main() {
    testBubbleSort();
    return 0;
}
