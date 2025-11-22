#include <iostream> 
#include <fstream>
using namespace std; 

#define n 4 

void interchangeFirstLast(int m[][n]) 
{ 
	for (int i = 0; i < n; i++) 
	{ 
		int t = m[i][0]; 
		m[i][0] = m[i][n - 1]; 
		m[i][n - 1] = t; 
	} 
} 

int main() 
{ 
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

	int m[n][n];
    int count = 0;

	while (count < 100 && infile >> m[0][0]) 
    { 
        // 讀取剩下的 15 個元素
        for (int i = 0; i < n; i++) 
        { 
            for (int j = 0; j < n; j++) 
            {
                // 跳過已經讀取的 m[0][0]
                if (i == 0 && j == 0) continue;
                infile >> m[i][j]; 
            }
        }

        // 執行交換
        interchangeFirstLast(m); 

        // 3. 輸出結果到 ans.txt
        // 將 4x4 矩陣拉平為一行輸出
        for (int i = 0; i < n; i++) 
        { 
            for (int j = 0; j < n; j++) 
            {
                // 判斷是否為矩陣的最後一個元素 (Row n-1, Col n-1)
                bool isLast = (i == n - 1) && (j == n - 1);
                outfile << m[i][j] << (isLast ? "" : " ");
            }
        }
        outfile << endl;

        count++;
    } 

    infile.close();
    outfile.close();
	return 0;
} 
