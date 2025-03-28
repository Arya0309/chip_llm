// C++ Program to implement linear search 
// algorithm iteratively
#include <bits/stdc++.h>
using namespace std;

int linearSearch(vector<int> v, int key) {
      
      // We test all the elements of the vector
      // v against the given key
      for (int i = 0; i < v.size(); i++) {
      
          // If the KEY IS FOUND
          if (v[i] == key) {
              return i;
        }
    }
      
      // Return some value denoting KEY NOT FOUND
      return -1;
}

int main() {
    vector<int> v = {1, 2, 3, 4, 5, 8, 9, 11};
  
    // Value to search
      int key = 8;

    // Searching the key in the vector v
      int i = linearSearch(v, key);
      
      // Checking if element is found or not
    if (i != -1)
        cout << key << " Found at Position: " <<
              i + 1;
    else
        cout << key << " NOT found.";
    return 0;
}
