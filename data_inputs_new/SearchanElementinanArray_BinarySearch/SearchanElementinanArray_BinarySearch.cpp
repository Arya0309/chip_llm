#include <bits/stdc++.h>
#include <fstream>
using namespace std;

bool binarySearch(vector<int>& v, int target) {
    
    int low = 0, high = v.size() - 1;
      
    while (low <= high) {
      
        int mid = ((high - low) / 2) + low;

        if (v[mid] == target) {
            return true;
        }

        if (v[mid] > target)
            high = mid - 1;
        
        else
            low = mid + 1;
    }
      
      return false;
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    const int N = 6;
    vector<int> v(N);
    int target;
    int count = 0;
    
    while (count < 100 && infile >> v[0]) {
        
        // 讀取陣列剩下的 7 個元素
        for (int i = 1; i < N; ++i) {
            infile >> v[i];
        }

        // 讀取目標值 target
        infile >> target;
        
        // 3. 執行搜尋並輸出結果
        if (binarySearch(v, target)) {
            outfile << "1" << endl;
        } else {
            outfile << "0" << endl;
        }
        
        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
