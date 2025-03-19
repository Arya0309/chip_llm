// C++ implementation of Naive method to print all divisors 
#include <iostream> 
using namespace std; 

// Function to print the divisors 
void printDivisors(int n) 
{ 
	for (int i = 1; i <= n; i++) 
		if (n % i == 0) 
			cout <<" " << i; 
} 

// Driver code 
int main() 
{ 
	cout <<"The divisors of 100 are: "; 
	printDivisors(100); 
	return 0; 
} 

// This code is contributed by shivanisinghss2110
