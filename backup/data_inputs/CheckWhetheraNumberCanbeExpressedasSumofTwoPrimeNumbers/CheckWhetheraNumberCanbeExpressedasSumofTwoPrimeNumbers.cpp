#include <bits/stdc++.h> 
using namespace std; 

bool isPrime(int n) 
{ 
	if (n <= 1) 
		return false; 

	for (int i = 2; i <= sqrt(n); i++) 
	{ 
		if (n % i == 0) 
			return false; 
	} 

	return true; 
} 

bool isPossible(int n) 
{ 
	for (int i = 2; i <= n / 2; i++) {
		if (isPrime(i) && isPrime(n - i)) {
			return true;
		}
	}
	return false;
} 

int main() 
{ 
	int n = 13; 

	if (isPossible(n)) 
		cout << "Yes"; 
	else
		cout << "No"; 

	return 0; 
} 
