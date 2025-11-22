#include <bits/stdc++.h>
#include <fstream>
using namespace std;

void Rotate(int arr[], int d, int n)
{
	int temp[n];
	int k = 0;

	for (int i = d; i < n; i++) {
		temp[k] = arr[i];
		k++;
	}

	for (int i = 0; i < d; i++) {
		temp[k] = arr[i];
		k++;
	}

	for (int i = 0; i < n; i++) {
		arr[i] = temp[i];
	}
}

int main()
{
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");
	const int N = 6;
    int d; 
    int arr[N];
    int count = 0;

	while (count < 100 && infile >> arr[0]) {
        
        // 讀取該行剩下的 6 個數字
        for (int i = 1; i < N; i++) {
            infile >> arr[i];
        }
        infile >> d;
        // 執行旋轉
        Rotate(arr, d, N);

        // 4. 輸出結果到 ans.txt
        for (int i = 0; i < N; i++) {
            outfile << arr[i] << (i == N - 1 ? "" : " ");
        }
        // 換行
        outfile << endl;

        count++;
    }
	infile.close();
    outfile.close();

	return 0;
}
