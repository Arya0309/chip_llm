#include "Dut.h"

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
        int arr[6] = {1, 2, 3, 4, 5, 6};
        int d = 2;
        int n = 6;
        /* === Variable Section End === */

        /* === Main function Section === */
        Rotate(arr, d, n);
        PrintTheArray(arr, n);
        /* === Main function Section End === */

        /* === Variable Section === */
        // No need to write back to any ports in this example
        /* === Variable Section End === */
    }
}
