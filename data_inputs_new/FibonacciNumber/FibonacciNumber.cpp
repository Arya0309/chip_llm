#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int fib(int n) {
    if (n <= 1)
        return n;
    
    int prev1 = 1, prev2 = 0;
    int curr;

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

    int n;
    int count = 0;

    while (count < 100 && infile >> n) {
        
        outfile << fib(n) << endl;
        
        count++;
    }

    infile.close();
    outfile.close();
    
    return 0;
}
