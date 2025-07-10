#include "Dut.h"

void printArray(int arr[], int len) {
	int i;
	for (i = 0; i < len; i++) {
		printf("%d ", arr[i]);
	}
}

void copyArrayReverse(int original_arr[], int copied_arr[], int len) {
	for (int i = 0; i < len; i++) {
		copied_arr[i] = original_arr[len - i - 1];
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
        int original_arr[6] = {1, 2, 3, 4, 5, 6};
        int len = sizeof(original_arr)/sizeof(original_arr[0]);
        int copied_arr[len];
        /* === Variable Section End === */

        /* === Main function Section === */
        copyArrayReverse(original_arr, copied_arr, len);

        printf(" Original array: ");
        printArray(original_arr, len);

        printf(" Resultant array: ");
        printArray(copied_arr, len);
        /* === Main function Section End === */
    }
}
