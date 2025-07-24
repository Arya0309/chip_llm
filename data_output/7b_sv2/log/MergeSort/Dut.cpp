
#include "Dut.h"

#include <vector>
using namespace std;

void merge(vector<int>& vec, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temporary vectors
    vector<int> leftVec(n1), rightVec(n2);

    // Copy data to temporary vectors
    for (i = 0; i < n1; i++)
        leftVec[i] = vec[left + i];
    for (j = 0; j < n2; j++)
        rightVec[j] = vec[mid + 1 + j];

    // Merge the temporary vectors back into vec[left..right]
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

    // Copy the remaining elements of leftVec[], if any
    while (i < n1) {
        vec[k] = leftVec[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightVec[], if any
    while (j < n2) {
        vec[k] = rightVec[j];
        j++;
        k++;
    }
}

void mergeSort(vector<int>& vec, int left, int right) {
    if (left < right) {
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        mergeSort(vec, left, mid);
        mergeSort(vec, mid + 1, right);

        // Merge the sorted halves
        merge(vec, left, mid, right);
    }
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
        int vec[6];
        // Read the array elements
        for (int i = 0; i < 6; ++i) {
            vec[i] = i_a.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        mergeSort(vec, 0, 5);
        /* === Main function Section End === */

        /* === Variable Section === */
        // Write the sorted array back out
        for (int i = 0; i < 6; ++i) {
            o_result.write(vec[i]);
        }
        /* === Variable Section End === */
    }
}
