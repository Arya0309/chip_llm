#include <bits/stdc++.h> 
#include <fstream>
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
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

	int n;
    int count = 0;

	while (count < 100 && infile >> n) 
    { 
        if (isPossible(n)) 
            outfile << "1" << endl; 
        else
            outfile << "0" << endl; 

        count++;
    }
	infile.close();
    outfile.close();
	return 0; 
} 
