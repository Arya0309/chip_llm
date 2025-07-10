#include "Dut.h"

void Rotate(int arr[], int d, int n) {
	// Storing rotated version of array
	int temp[n];

	// Keeping track of the current index
	// of temp[]
	int k = 0;

	// Storing the n - d elements of
	// array arr[] to the front of temp[]
	for (int i = d; i < n; i++) {
		temp[k] = arr[i];
		k++;
	}

	// Storing the first d elements of array arr[]
	// into temp
	for (int i = 0; i < d; i++) {
		temp[k] = arr[i];
		k++;
	}

	// Copying the elements of temp[] in arr[]
	// to get the final rotated array
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
        int arr[6] = {i_arr0.read(), i_arr1.read(), i_arr2.read(), i_arr3.read(), i_arr4.read(), i_arr5.read()};
        int d = i_d.read();
        int n = 6;
        /* === Variable Section End === */

        /* === Main function Section === */
        Rotate(arr, d, n);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result0.write(arr[0]);
        o_result1.write(arr[1]);
        o_result2.write(arr[2]);
        o_result3.write(arr[3]);
        o_result4.write(arr[4]);
        o_result5.write(arr[5]);
        /* === Variable Section End === */
    }
}
