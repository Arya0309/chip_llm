#include <stdio.h>
#include <fstream>
using namespace std;
void printArray(int arr[], int len) {
	for (int i = 0; i < len; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void reverseArray(int original[], int reversed[], int len) {
	for (int i = 0; i < len; i++) {
		reversed[i] = original[len - i - 1];
	}
}

int main() {
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

	const int LEN = 6; 
    int original_arr[LEN];
    int copied_arr[LEN];
    int count = 0;

	while (count < 100 && infile >> original_arr[0]) {
        
        // 讀取該行剩下的 4 個數字
        for (int i = 1; i < LEN; i++) {
            infile >> original_arr[i];
        }

        // 執行反轉
        reverseArray(original_arr, copied_arr, LEN);

        // 4. 輸出結果到 ans.txt
        // 只輸出數值，不輸出 "Resultant array:" 等文字
        for (int i = 0; i < LEN; i++) {
            outfile << copied_arr[i] << (i == LEN - 1 ? "" : " ");
        }
        outfile << endl; // 換行

        count++;
    }

    infile.close();
    outfile.close();

	return 0;
}
