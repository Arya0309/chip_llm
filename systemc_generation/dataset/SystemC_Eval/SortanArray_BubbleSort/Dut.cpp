#include "Dut.h"

static constexpr int N = 6;

void swap(int* arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void bubbleSort(int arr[]) {
    for (int i = 0; i < N - 1; i++) {

        // Last i elements are already in place, so the loop
        // will only num N - i - 1 times
        for (int j = 0; j < N - i - 1; j++) {
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
        // Perform bubble sort on the array.
        bubbleSort(arr);

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
