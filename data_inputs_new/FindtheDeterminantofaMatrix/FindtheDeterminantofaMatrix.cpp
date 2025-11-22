#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

const int MAXN = 105;
double a[MAXN][MAXN];

double determinant(int n) {
	double det = 1.0;
	for (int i = 0; i < n; i++) {
		int pivot = i;
		for (int j = i + 1; j < n; j++) {
			if (abs(a[j][i]) > abs(a[pivot][i])) {
				pivot = j;
			}
		}
		if (pivot != i) {
			swap(a[i], a[pivot]);
			det *= -1;
		}
		if (a[i][i] == 0) {
			return 0;
		}
		det *= a[i][i];
		for (int j = i + 1; j < n; j++) {
			double factor = a[j][i] / a[i][i];
			for (int k = i + 1; k < n; k++) {
				a[j][k] -= factor * a[i][k];
			}
		}
	}
	return det;
}

int main() {
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");
	int n = 4;
	int count = 0;
	while (count < 100 && infile >> a[0][0]) {
        
        // 讀取第一列剩下的元素
        for (int j = 1; j < n; j++) {
            infile >> a[0][j];
        }

        // 讀取剩下的 3 列 (Row 1 ~ 3)
        for (int i = 1; i < n; i++) {
            for (int j = 0; j < n; j++) {
                infile >> a[i][j];
            }
        }

        // 計算行列式
        // 注意：determinant 函式會破壞原本的 a 陣列進行高斯消去，
        // 但因為下一輪迴圈會重新讀取覆蓋 a，所以沒關係。
        double det = determinant(n);
        
        // 輸出結果 (可能會有極小的誤差，例如 -0，視需求可做處理，這裡直接輸出)
        outfile << det << endl;
        
        count++;
    }

    infile.close();
    outfile.close();
	return 0;
}
