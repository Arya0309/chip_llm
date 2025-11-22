#include <iostream>
#include <fstream>
using namespace std;

bool isPrime(int n) {
    if (n <= 1)
        return false;

    int cnt = 0;
    for (int i = 1; i <= n; i++) {
        if (n % i == 0)
            cnt++;
    }
    return cnt == 2;
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    int n;
    int count = 0;

    // 2. 迴圈讀取 100 筆資料
    while (count < 100 && infile >> n) {
        if (isPrime(n))
            outfile << "1" << endl;
        else
            outfile << "0" << endl;
        
        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
