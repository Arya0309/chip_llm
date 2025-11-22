#include <iostream> 
#include <fstream>
#define R 4 
#define C 4 
using namespace std; 

void rotatematrix(int m, int n, 
				int mat[R][C]) 
{ 
	int row = 0, col = 0; 
	int prev, curr; 

	while (row < m && col < n) 
	{ 
		if (row + 1 == m || 
			col + 1 == n) 
			break; 

		prev = mat[row + 1][col]; 

		for (int i = col; i < n; i++) 
		{ 
			curr = mat[row][i]; 
			mat[row][i] = prev; 
			prev = curr; 
		} 
		row++; 

		for (int i = row; i < m; i++) 
		{ 
			curr = mat[i][n-1]; 
			mat[i][n-1] = prev; 
			prev = curr; 
		} 
		n--; 

		if (row < m) 
		{ 
			for (int i = n-1; i >= col; i--) 
			{ 
				curr = mat[m-1][i]; 
				mat[m-1][i] = prev; 
				prev = curr; 
			} 
		} 
		m--; 

		if (col < n) 
		{ 
			for (int i = m-1; i >= row; i--) 
			{ 
				curr = mat[i][col]; 
				mat[i][col] = prev; 
				prev = curr; 
			} 
		} 
		col++; 
	} 
} 

int main() 
{ 
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

	int a[R][C];
    int count = 0;

    // 2. 迴圈讀取 100 筆資料
    // 嘗試讀取矩陣的第一個元素 a[0][0]
    while (count < 100 && infile >> a[0][0]) 
    { 
        // 讀取剩下的 15 個元素
        for (int i = 0; i < R; i++) 
        { 
            for (int j = 0; j < C; j++) 
            {
                // 跳過已經讀取的 a[0][0]
                if (i == 0 && j == 0) continue;
                infile >> a[i][j]; 
            }
        }

        // 執行旋轉計算
        rotatematrix(R, C, a); 

        // 3. 輸出結果到 ans.txt
        // 將 4x4 結果拉平印在同一行
        for (int i = 0; i < R; i++) 
        { 
            for (int j = 0; j < C; j++) 
            {
                outfile << a[i][j];
                // 判斷是否為最後一個元素，避免尾端空白
                if (!(i == R - 1 && j == C - 1)) {
                    outfile << " ";
                }
            }
        }
        outfile << endl; // 該筆資料結束後換行

        count++;
    } 

    infile.close();
    outfile.close();
	return 0; 
} 
