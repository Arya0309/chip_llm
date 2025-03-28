// C++ Program to find sum of 
// even-indiced Fibonacci numbers
#include <bits/stdc++.h>
using namespace std;

// Computes value of first 
// fibonacci numbers and 
// stores the even-indexed sum
int calculateEvenSum(int n)
{
	if (n <= 0)
		return 0;

	int fibo[2 * n + 1];
	fibo[0] = 0, fibo[1] = 1;

	// Initialize result
	int sum = 0;

	// Add remaining terms
	for (int i = 2; i <= 2 * n; i++) 
	{
		fibo[i] = fibo[i - 1] + fibo[i - 2];

		// For even indices
		if (i % 2 == 0)
			sum += fibo[i];
	}

	// Return the alternating sum
	return sum;
}

// Driver code
int main()
{

	// Get n
	int n = 8;

	// Find the even-indiced sum
	cout << "Even indexed Fibonacci Sum upto " << 
			n << " terms: " << calculateEvenSum(n) << 
			endl;

	return 0;
}
