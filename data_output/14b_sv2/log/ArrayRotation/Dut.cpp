
#include "Dut.h"

constexpr int N = 6;

// original rotate function
void Rotate(int arr[], int d, int n) {
    int temp[n];
    int k = 0;
    for (int i = d; i < n; i++) {
        temp[k] = arr[i];
        k++;
    }
    for (int i = 0; i < d; i++) {
        temp[k] = arr[i];
        k++;
    }
    for (int i = 0; i < n; i++) {
        arr[i] = temp[i];
    }
}

// original print function
void PrintTheArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
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
        int arr[N];
        int d = i_d.read();
        // read array element-by-element
        for (int i = 0; i < N; ++i) {
            arr[i] = i_arr.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        Rotate(arr, d, N);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write result array back out
        for (int i = 0; i < N; ++i) {
            o_result.write(arr[i]);
        }
        /* === Variable Section End === */
    }
}
