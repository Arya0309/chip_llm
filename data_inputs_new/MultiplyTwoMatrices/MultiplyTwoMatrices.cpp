#include <bits/stdc++.h>
#include <fstream>
using namespace std;

#define R1 2
#define C1 2
#define R2 2
#define C2 3

void mulMat(int mat1[][C1], int mat2[][C2], int rslt[][C2])
{
    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            rslt[i][j] = 0;
            for (int k = 0; k < R2; k++) {
                rslt[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}
int main()
{
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

	int mat1[R1][C1];
    int mat2[R2][C2];
	int rslt[R1][C2]; // 用來存放結果的陣列
    int count = 0;

	while (count < 100 && infile >> mat1[0][0]) {
        
        // 讀取 mat1 剩下的 3 個元素
        for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C1; j++) {
                if (i == 0 && j == 0) continue; // 跳過已讀取的第一個
                infile >> mat1[i][j];
            }
        }

        // 讀取 mat2 的 6 個元素
        for (int i = 0; i < R2; i++) {
            for (int j = 0; j < C2; j++) {
                infile >> mat2[i][j];
            }
        }

        // 執行運算並輸出
        mulMat(mat1, mat2, rslt);
		for (int i = 0; i < R1; i++) {
            for (int j = 0; j < C2; j++) {
                outfile << rslt[i][j];
                
                // 判斷是否為最後一個元素，避免尾端多餘空白
                if (!(i == R1 - 1 && j == C2 - 1)) {
                    outfile << " ";
                }
            }
        }
        outfile << endl;

        count++;
    }

    infile.close();
    outfile.close();
	return 0;
}
