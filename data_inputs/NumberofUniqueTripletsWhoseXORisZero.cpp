// CPP program to count the number of
// unique triplets whose XOR is 0
#include <bits/stdc++.h>
using namespace std;

// function to count the number of 
// unique triplets whose xor is 0
int countTriplets(int a[], int n) 
{
	// To store values that are present
	unordered_set<int> s;
	for (int i = 0; i < n; i++)
		s.insert(a[i]);
	
	// stores the count of unique triplets
	int count = 0;
	
	// traverse for all i, j pairs such that j>i
	for (int i = 0; i < n-1; i++) {
		for (int j = i + 1; j < n; j++) {

		// xor of a[i] and a[j]
		int xr = a[i] ^ a[j];
	
		// if xr of two numbers is present, 
		// then increase the count
		if (s.find(xr) != s.end() && xr != a[i] && 
									xr != a[j])
			count++;
		}
	}
	
	// returns answer
	return count / 3;
}

// Driver code to test above function
int main() 
{
	int a[] = {1, 3, 5, 10, 14, 15};
	int n = sizeof(a) / sizeof(a[0]); 
	cout << countTriplets(a, n); 
	return 0;
}
