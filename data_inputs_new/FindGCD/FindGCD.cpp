#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int gcd(int a, int b) {
    int res = min(a, b);

    while (res > 1) {
        if (a % res == 0 && b % res == 0)
            break;
        res--;
    }
    return res;
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");
    int a, b;
    int count = 0;
  
    while (count < 100 && infile >> a >> b) {
        
        outfile << gcd(a, b) << endl;

        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
