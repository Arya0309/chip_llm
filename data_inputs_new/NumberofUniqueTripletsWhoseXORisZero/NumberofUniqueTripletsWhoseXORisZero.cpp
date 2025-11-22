#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int countTriplets(int a[], int n) 
{
	unordered_set<int> s;
	for (int i = 0; i < n; i++)
		s.insert(a[i]);
	
	int count = 0;
	
	for (int i = 0; i < n-1; i++) {
		for (int j = i + 1; j < n; j++) {

		int xr = a[i] ^ a[j];
	
		if (s.find(xr) != s.end() && xr != a[i] && 
									xr != a[j])
			count++;
		}
	}
	
	return count / 3;
}

int main() 
{
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

	const int n = 6;
    int a[n];
    int count = 0;

	while (count < 100 && infile >> a[0]) {
        
        // 讀取剩下的 5 個整數
        for (int i = 1; i < n; i++) {
            infile >> a[i];
        }

        // 計算並輸出結果
        outfile << countTriplets(a, n) << endl;
        
        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
