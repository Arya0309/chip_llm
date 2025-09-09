const int NUM_TRAIN = 6;
const int DIM = 3;
const int K = 3;

int knn(const double train[][DIM], const int labels[], int num_train, const double query_point[]) {
    double dist[NUM_TRAIN];
    int idx[NUM_TRAIN];

    // Compute distances for the first num_train points; others get a large distance
    for (int i = 0; i < NUM_TRAIN; i++) {
        if (i < num_train) {
            double sum = 0.0;
            for (int d = 0; d < DIM; d++) {
                double diff = train[i][d] - query_point[d];
                sum += diff * diff;
            }
            dist[i] = sum;
            idx[i] = i;
        } else {
            dist[i] = 1e12;   // effectively infinite distance
            idx[i] = -1;
        }
    }

    // Selection sort to find K smallest distances
    for (int i = 0; i < K; i++) {
        int min_j = i;
        for (int j = i + 1; j < NUM_TRAIN; j++) {
            if (j < num_train && dist[j] < dist[min_j]) {
                min_j = j;
            }
        }
        double tmpDist = dist[i]; dist[i] = dist[min_j]; dist[min_j] = tmpDist;
        int tmpIdx = idx[i];   idx[i]   = idx[min_j];   idx[min_j]   = tmpIdx;
    }

    int count0 = 0, count1 = 0;
    for (int i = 0; i < K; i++) {
        if (idx[i] >= 0) {            // guard against invalid indices
            int label = labels[idx[i]];
            if (label == 0)      count0++;
            else if (label == 1) count1++;
        }
    }

    return (count1 > count0) ? 1 : 0;
}