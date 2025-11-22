#include <bits/stdc++.h>
using namespace std;

bool binarySearch(vector<int>& v, int target) {
    
    int low = 0, high = v.size() - 1;
      
    while (low <= high) {
      
        int mid = ((high - low) / 2) + low;

        if (v[mid] == target) {
            return true;
        }

        if (v[mid] > target)
            high = mid - 1;
        
        else
            low = mid + 1;
    }
      
      return false;
}

int main() {
    vector<int> v = {1, 2, 3, 4, 5, 8, 9, 11};
    
    int target = 8;
  
    if (binarySearch(v, target)) {
        cout << target << " found.";
    } else {
        cout << target << " NOT found.";
    }
    return 0;
}
