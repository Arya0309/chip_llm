
#include "Dut.h"

void swap(int* arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(arr, j, j + 1);
        }
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
        int n = i_n.read();
        int arr[n];
        for (int i = 0; i < n; i++) {
            arr[i] = i_arr[i].read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        bubbleSort(arr, n);
        /* === Main function Section End === */

        /* === Variable Section === */
        for (int i = 0; i < n; i++) {
            o_arr[i].write(arr[i]);
        }
        /* === Variable Section End === */
    }
}
