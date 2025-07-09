
#include "Dut.h"

#include <vector>
using namespace std;

bool binarySearch(vector<int>& v, int target) {
    int low = 0, high = v.size() - 1;
    while (low <= high) {
        int mid = ((high - low) / 2) + low;
        if (v[mid] == target) {
            return true;
        }
        if (v[mid] > target)
            high = mid - 1;
        else
            low = mid + 1;
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
        vector<int> v = i_v.read();
        int target = i_target.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        bool res = binarySearch(v, target);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
