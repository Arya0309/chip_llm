#include <bits/stdc++.h>
#include <fstream>
using namespace std;

long long int fib(long long n) {
    if (n <= 1)
        return n;
    
    long long prev1 = 1, prev2 = 0;
    long long curr;

    for (int i = 2; i <= n; i++) {
        curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return curr;
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    long long n;
    int count = 0;

    while (count < 100 && infile >> n) {
        
        outfile << fib(n) << endl;
        
        count++;
    }

    infile.close();
    outfile.close();
    
    return 0;
}
