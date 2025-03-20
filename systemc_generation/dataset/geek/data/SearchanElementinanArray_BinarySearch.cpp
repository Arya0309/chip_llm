// C++ program to implement iterative 
// binary search
#include <bits/stdc++.h>
using namespace std;

bool binarySearch(vector<int>& v, int target) {
    
      // Defining the part of the vector to be
      // searched
    int low = 0, high = v.size() - 1;
      
      // Till the element is found or vector cannot
      // be divided into more parts
    while (low <= high) {
      
          // Finding mid point
        int mid = ((high - low) / 2) + low;

        // If the middle element is equal to target
        if (v[mid] == target) {
            return true;
        }

        // If the middle element is greater than 
        // target, search in the left half 
        if (v[mid] > target)
            high = mid - 1;
        
        // If the middle element is smaller than
        // target, search the right half 
        else
            low = mid + 1;
    }
      
      // If we don't find the target
      return false;
}

int main() {
    vector<int> v = {1, 2, 3, 4, 5, 8, 9, 11};
    
      // Element to be searched
      int target = 8;
  
      // Searching the target element
    if (binarySearch(v, target)) {
        cout << target << " found.";
    } else {
        cout << target << " NOT found.";
    }
    return 0;
}
