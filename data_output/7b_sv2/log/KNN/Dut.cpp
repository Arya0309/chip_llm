
#include "Dut.h"

#define DIM 3
#define NUM_TRAIN 6
#define K 3

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
        double dist[NUM_TRAIN];
        int idx[NUM_TRAIN];
        int count0 = 0, count1 = 0;
        /* === Variable Section End === */

        /* === Main function Section === */
        // Read input data from FIFOs
        for (int i = 0; i < NUM_TRAIN; ++i) {
            for (int d = 0; d < DIM; ++d) {
                train[i][d] = i_train.read();
            }
            labels[i] = i_labels.read();
        }
        for (int d = 0; d < DIM; ++d) {
            query_point[d] = i_query_point.read();
        }

        // 1. Calculate squared distances and initialize indices
        for (int i = 0; i < NUM_TRAIN; i++) {
            double sum = 0.0;
            for (int d = 0; d < DIM; d++) {
                double diff = train[i][d] - query_point[d];
                sum += diff * diff;
            }
            dist[i] = sum;
            idx[i] = i;
        }

        // 2. Partial sort: move the smallest K distances to the first K positions
        for (int i = 0; i < K; i++) {
            int min_j = i;
            for (int j = i + 1; j < NUM_TRAIN; j++) {
                if (dist[j] < dist[min_j]) {
                    min_j = j;
                }
            }
            // Swap distances and indices
            double tmpDist = dist[i]; dist[i] = dist[min_j]; dist[min_j] = tmpDist;
            int tmpIdx = idx[i];   idx[i]   = idx[min_j];   idx[min_j]   = tmpIdx;
        }

        // 3. Count occurrences of each label among the K neighbors
        count0 = 0;
        count1 = 0;
        for (int i = 0; i < K; i++) {
            int label = labels[idx[i]];
            if (label == 0)      count0++;
            else if (label == 1) count1++;
        }

        // 4. Majority vote
        int result = (count1 > count0) ? 1 : 0;

        // Write result back to FIFO
        o_result.write(result);
        /* === Main function Section End === */
    }
}
