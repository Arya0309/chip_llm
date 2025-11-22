// C++ program to find common elements between two Arrays using set_intersection()
#include <bits/stdc++.h>
using namespace std;

int main() {
    int arr1[] = {1, 45, 54, 71, 76, 12};
    int arr2[] = {1, 7, 5, 4, 6, 12};

    // Compute the sizes
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int n2 = sizeof(arr2) / sizeof(arr2[0]);

    // Sort the arrays
    sort(arr1, arr1 + n1);
    sort(arr2, arr2 + n2);

    // Vector to store result
    vector<int> v;
    
    // Finding common elements using
    // set_intersection() function
    set_intersection(arr1, arr1 + n1, arr2,
        arr2 + n2, back_inserter(v));

    for (auto i: v){
        cout << i << " ";
    }
    return 0;
}
