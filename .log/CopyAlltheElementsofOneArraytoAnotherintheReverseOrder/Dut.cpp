
#include "Dut.h"

void printArray(int arr[], int len) {
    int i;
    for (i = 0; i < len; i++) {
        printf("%d ", arr[i]);
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
        int len = i_len.read();
        int* arr = new int[len];
        for (int j = 0; j < len; j++) {
            arr[j] = i_arr[j].read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        printArray(arr, len);
        /* === Main function Section End === */

        /* === Variable Section === */
        delete[] arr;
        /* === Variable Section End === */
    }
}
