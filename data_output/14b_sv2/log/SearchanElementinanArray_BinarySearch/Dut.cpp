
#include "Dut.h"

bool binarySearch(const int v[], int target, int size) {
    int low = 0, high = size - 1;
    while (low <= high) {
        int mid = ((high - low) / 2) + low;
        if (v[mid] == target) {
            return true;
        }
        if (v[mid] > target) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return false;
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
        int v[6];
        for (int i = 0; i < 6; ++i) {
            v[i] = i_array.read();
        }
        int target = i_target.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        bool result = binarySearch(v, target, 6);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(result);
        /* === Variable Section End === */
    }
}
