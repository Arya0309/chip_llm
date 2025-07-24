
#include "Dut.h"

constexpr int N = 6;

// Function to print an array
void printArray(int arr[], int len) {
    for(int i = 0; i < len; i++) {
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
        int original_arr[N];
        // Read the original array element-by-element
        for (int i = 0; i < N; ++i) {
            original_arr[i] = i_a.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        int copied_arr[N];
        // Copy the elements of the array in reverse order
        for (int i = 0; i < N; i++) {
            copied_arr[i] = original_arr[N - i - 1];
        }

        // Print the original array
        printf(" Original array: ");
        printArray(original_arr, N);

        // Print the copied array
        printf(" Resultant array: ");
        printArray(copied_arr, N);
        /* === Main function Section End === */
    }
}
