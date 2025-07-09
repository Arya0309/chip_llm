#include <iostream>
using namespace std;

// 訓練資料筆數、維度、K 值
const int NUM_TRAIN = 6;
const int DIM = 3;
const int K = 3;

// KNN 函式：回傳預測的整數標籤（假設 labels 只有 0 或 1）
int knn(const double train[][DIM], const int labels[], int num_train, const double query_point[]) {
    double dist[NUM_TRAIN];
    int idx[NUM_TRAIN];

    // 1. 計算每個訓練點到查詢點的距離平方，並初始化索引
    for (int i = 0; i < num_train; i++) {
        double sum = 0.0;
        for (int d = 0; d < DIM; d++) {
            double diff = train[i][d] - query_point[d];
            sum += diff * diff;
        }
        dist[i] = sum;
        idx[i] = i;
    }

    // 2. 部分排序：把最小的 K 個距離放到陣列前 K 個位置
    for (int i = 0; i < K; i++) {
        int min_j = i;
        for (int j = i + 1; j < num_train; j++) {
            if (dist[j] < dist[min_j]) {
                min_j = j;
            }
        }
        // 交換距離與對應索引
        double tmpDist = dist[i]; dist[i] = dist[min_j]; dist[min_j] = tmpDist;
        int tmpIdx = idx[i];   idx[i]   = idx[min_j];   idx[min_j]   = tmpIdx;
    }

    // 3. 統計 K 個鄰居中各標籤出現次數（假設只有 0/1）
    int count0 = 0, count1 = 0;
    for (int i = 0; i < K; i++) {
        int label = labels[idx[i]];
        if (label == 0)      count0++;
        else if (label == 1) count1++;
    }

    // 4. 多數決
    return (count1 > count0) ? 1 : 0;
}

int main() {
    // 範例訓練資料：6 筆 3 維向量
    double train_data[NUM_TRAIN][DIM] = {
        {1.0, 2.0, 1.5},
        {2.0, 1.0, 2.5},
        {0.5, 1.5, 1.0},
        {3.0, 3.5, 2.0},
        {2.5, 2.0, 3.0},
        {3.5, 0.5, 2.5}
    };
    int train_labels[NUM_TRAIN] = {0, 0, 0, 1, 1, 1};

    // 查詢點
    double query[DIM] = {2.0, 2.0, 2.0};

    // 呼叫 KNN
    int result = knn(train_data, train_labels, NUM_TRAIN, query);

    cout << "Predicted label: " << result << endl;
    return 0;
}
