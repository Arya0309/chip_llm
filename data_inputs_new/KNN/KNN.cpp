#include <iostream>
#include <fstream>
using namespace std;

const int NUM_TRAIN = 6;
const int DIM = 3;
const int K = 3;

int knn(const double train[][DIM], const int labels[], int num_train, const double query_point[]) {
    double dist[NUM_TRAIN];
    int idx[NUM_TRAIN];

    for (int i = 0; i < num_train; i++) {
        double sum = 0.0;
        for (int d = 0; d < DIM; d++) {
            double diff = train[i][d] - query_point[d];
            sum += diff * diff;
        }
        dist[i] = sum;
        idx[i] = i;
    }

    for (int i = 0; i < K; i++) {
        int min_j = i;
        for (int j = i + 1; j < num_train; j++) {
            if (dist[j] < dist[min_j]) {
                min_j = j;
            }
        }
        double tmpDist = dist[i]; dist[i] = dist[min_j]; dist[min_j] = tmpDist;
        int tmpIdx = idx[i];   idx[i]   = idx[min_j];   idx[min_j]   = tmpIdx;
    }

    int count0 = 0, count1 = 0;
    for (int i = 0; i < K; i++) {
        int label = labels[idx[i]];
        if (label == 0)      count0++;
        else if (label == 1) count1++;
    }

    return (count1 > count0) ? 1 : 0;
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    double train_data[NUM_TRAIN][DIM];
    int train_labels[NUM_TRAIN];
    double query[DIM];

    int count = 0;

    while (count < 100 && infile >> train_data[0][0]) {
        
        // 讀取 train_data 的第 1 個點剩下的維度
        for (int d = 1; d < DIM; d++) infile >> train_data[0][d];

        // 讀取 train_data 剩下的 5 個點 (共 15 個數)
        for (int i = 1; i < NUM_TRAIN; i++) {
            for (int d = 0; d < DIM; d++) {
                infile >> train_data[i][d];
            }
        }

        // 讀取 train_labels (6 個數)
        for (int i = 0; i < NUM_TRAIN; i++) {
            infile >> train_labels[i];
        }

        // 讀取 query (3 個數)
        for (int d = 0; d < DIM; d++) {
            infile >> query[d];
        }

        // 執行 KNN
        int result = knn(train_data, train_labels, NUM_TRAIN, query);

        // 3. 輸出結果到 ans.txt
        outfile << result << endl;

        count++;
    }

    infile.close();
    outfile.close();
    return 0;
}
