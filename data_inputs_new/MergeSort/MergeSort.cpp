#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

void merge(vector<int>& vec, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> leftVec(n1), rightVec(n2);

    for (i = 0; i < n1; i++)
        leftVec[i] = vec[left + i];
    for (j = 0; j < n2; j++)
        rightVec[j] = vec[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (leftVec[i] <= rightVec[j]) {
            vec[k] = leftVec[i];
            i++;
        } else {
            vec[k] = rightVec[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = leftVec[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = rightVec[j];
        j++;
        k++;
    }
}

void mergeSort(vector<int>& vec, int left, int right) {
    if (left < right) {
      
        int mid = left + (right - left) / 2;

        mergeSort(vec, left, mid);
        mergeSort(vec, mid + 1, right);

        merge(vec, left, mid, right);
    }
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");
    int n = 6;
    vector<int> vec(n);
    int count = 0;
    while (count < 100 && infile >> vec[0]) {
        for (int i = 1; i < n; i++) {
            infile >> vec[i];
        }
        mergeSort(vec, 0, n - 1);
        for (int i = 0; i < n; i++) {
            outfile << vec[i] << (i == n - 1 ? "" : " ");
        }
        outfile << endl;
        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
