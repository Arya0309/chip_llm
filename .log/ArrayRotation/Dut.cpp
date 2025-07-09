
#include "Dut.h"

void Rotate(int arr[], int d, int n)
{
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

void PrintTheArray(int arr[], int n)
{
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
        int d = i_d.read();
        int n = i_n.read();
        int arr[n];
        for (int i = 0; i < n; i++) {
            arr[i] = i_arr[i].read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        Rotate(arr, d, n);
        /* === Main function Section End === */

        /* === Variable Section === */
        for (int i = 0; i < n; i++) {
            o_arr[i].write(arr[i]);
        }
        /* === Variable Section End === */
    }
}
