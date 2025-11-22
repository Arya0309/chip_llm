#include "Dut.h"

static constexpr int N = 6;

static void mergeSortIter(int arr[N]) {
    int tmp[N];
    for (int width = 1; width < N; width <<= 1) {
        for (int left = 0; left < N; left += 2*width) {
            int sum1 = left + width;
            int sum2 = left + 2*width;
            int mid   = (sum1 < N ? sum1 : N);
            int right = (sum2 < N ? sum2 : N);
            int l = left, r = mid, k = left;
            while (l < mid && r < right) {
                if (arr[l] <= arr[r]) tmp[k++] = arr[l++];
                else                  tmp[k++] = arr[r++];
            }
            while (l < mid)        tmp[k++] = arr[l++];
            while (r < right)      tmp[k++] = arr[r++];
        }
        for (int i = 0; i < N; ++i)
            arr[i] = tmp[i];
    }
}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */

#ifndef NATIVE_SYSTEMC
    i_in_array.clk_rst(i_clk, i_rst);
    o_out_array.clk_rst(i_clk, i_rst);
#endif

}

void Dut::do_compute() {

    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        i_in_array.reset();
        o_out_array.reset();
#endif
        wait();
    }

    while (true) {

        // Read input array into a local array.
        int arr[6];
#ifndef NATIVE_SYSTEMC
        for (int i = 0; i < 6; i++) {
            arr[i] = i_in_array.get();
        }
#else
        for (int i = 0; i < 6; i++) {
            arr[i] = i_in_array.read();
        }
#endif
        // Perform merge sort on the array.
        mergeSortIter(arr);

        // Write the rotated array to output ports.
        for (int i = 0; i < 6; i++) {        
#ifndef NATIVE_SYSTEMC
		    o_out_array.put(arr[i]);
#else
		    o_out_array.write(arr[i]);
#endif
        }

    }

}
