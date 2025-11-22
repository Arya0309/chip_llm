#include <iostream>
#include <fstream>
using namespace std;

int findLCM(int a, int b) {
    int max_num = (a > b) ? a : b;

    while (true) {
        if (max_num % a == 0 && max_num % b == 0) {
            return max_num;
        }
        ++max_num;
    }
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    int a, b;
    int count = 0;

    while (count < 100 && infile >> a >> b) {
        
        int lcm = findLCM(a, b);
        
        // 輸出純數值結果到 ans.txt
        outfile << lcm << endl;
        
        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
