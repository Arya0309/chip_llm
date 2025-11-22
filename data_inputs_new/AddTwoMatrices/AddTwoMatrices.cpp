#include <bits/stdc++.h>
#include <fstream>
using namespace std;
#define N 4

void add(int A[][N], int B[][N], int C[][N])
{
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            C[i][j] = A[i][j] + B[i][j];
}

int main()
{
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    int A[N][N], B[N][N], C[N][N];
    int count = 0;

    while (count < 100 && infile >> A[0][0]) {
        
        // 讀取矩陣 A 剩下的 15 個元素
        // 注意：因為 A[0][0] 已經讀過了，迴圈要避開它
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (i == 0 && j == 0) continue; 
                infile >> A[i][j];
            }
        }

        // 讀取矩陣 B 的 16 個元素
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                infile >> B[i][j];
            }
        }

        // 執行相加
        add(A, B, C);

        // 輸出結果到 ans.txt
        // 將 4x4 矩陣結果拉平在同一行輸出
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                // 判斷是否為最後一個元素 (Row N-1, Col N-1)
                bool isLast = (i == N - 1) && (j == N - 1);
                outfile << C[i][j] << (isLast ? "" : " ");
            }
        }
        outfile << endl;

        count++;
    }
    infile.close();
    outfile.close();
    return 0;
}
