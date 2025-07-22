#include <iostream>
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
    double train_data[NUM_TRAIN][DIM] = {
        {1.0, 2.0, 1.5},
        {2.0, 1.0, 2.5},
        {0.5, 1.5, 1.0},
        {3.0, 3.5, 2.0},
        {2.5, 2.0, 3.0},
        {3.5, 0.5, 2.5}
    };
    int train_labels[NUM_TRAIN] = {0, 0, 0, 1, 1, 1};

    double query[DIM] = {2.0, 2.0, 2.0};

    int result = knn(train_data, train_labels, NUM_TRAIN, query);

    cout << "Predicted label: " << result << endl;
    return 0;
}
