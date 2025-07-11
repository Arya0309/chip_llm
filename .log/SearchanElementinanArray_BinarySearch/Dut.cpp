#include "Dut.h"

bool binarySearch(sc_vector<int>& v, int target) {
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
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
}

void Dut::do_compute() {
    wait();
    while (true) {
        sc_vector<int> v(6);
        for (int i = 0; i < 6; ++i) {
            v[i] = i_a[i].read();
        }
        int target = i_target.read();
        bool res = binarySearch(v, target);
        o_result.write(res);
    }
}
