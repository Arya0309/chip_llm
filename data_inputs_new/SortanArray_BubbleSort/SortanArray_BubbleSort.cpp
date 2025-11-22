#include <stdio.h>
#include <fstream>
using namespace std;

void swap(int* arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
      
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(arr, j, j + 1);
        }
    }
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    const int n = 6;
    int arr[n];
    int count = 0;

    // 2. 迴圈讀取 100 筆資料
    // 每次讀取 4 個整數
    while (count < 100 && infile >> arr[0]) {
        
        // 讀取剩下的 3 個數字
        for (int i = 1; i < n; i++) {
            infile >> arr[i];
        }

        // 執行排序
        bubbleSort(arr, n);

        // 3. 輸出結果到 ans.txt
        // 將 4 個排序後的數字印在同一行
        for (int i = 0; i < n; i++) {
            outfile << arr[i] << (i == n - 1 ? "" : " ");
        }
        outfile << endl;

        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
