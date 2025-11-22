#include <bits/stdc++.h>
#include <fstream> 
using namespace std; 

int pow(int a, int b) 
{ 
	if (b == 0) 
		return 1; 
	int answer = a; 
	int increment = a; 
	int i, j; 
	for(i = 1; i < b; i++) 
	{ 
		for(j = 1; j < a; j++) 
		{ 
			answer += increment; 
		} 
		increment = answer; 
	} 
	return answer; 
} 

int main() 
{ 
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

	int a, b;
    int count = 0;

    while (count < 100 && infile >> a >> b) 
    { 
        outfile << pow(a, b) << endl; 
        count++;
    }
	infile.close();
    outfile.close();
	return 0; 
} 
