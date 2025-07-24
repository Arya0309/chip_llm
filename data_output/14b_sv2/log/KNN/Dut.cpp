
#include "Dut.h"

constexpr int DIM = 3;
constexpr int NUM_TRAIN = 6;
constexpr int K = 3;

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

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        double train[NUM_TRAIN][DIM];
        int labels[NUM_TRAIN];
        double query_point[DIM];

        // Read train matrix
        for (int i = 0; i < NUM_TRAIN; i++) {
            for (int d = 0; d < DIM; d++) {
                train[i][d] = i_train.read();
            }
        }

        // Read labels array
        for (int i = 0; i < NUM_TRAIN; i++) {
            labels[i] = i_labels.read();
        }

        // Read query point array
        for (int d = 0; d < DIM; d++) {
            query_point[d] = i_query_point.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        int result = knn(train, labels, NUM_TRAIN, query_point);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(result);
        /* === Variable Section End === */
    }
}
