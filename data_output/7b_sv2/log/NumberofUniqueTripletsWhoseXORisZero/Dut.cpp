
#include "Dut.h"

#include <unordered_set>
using namespace std;

int countTriplets(int a[], int n) {
    unordered_set<int> s;
    for (int i = 0; i < n; i++)
        s.insert(a[i]);

    int count = 0;

    for (int i = 0; i < n-1; i++) {
        for (int j = i + 1; j < n; j++) {
            int xr = a[i] ^ a[j];
            if (s.find(xr) != s.end() && xr != a[i] && xr != a[j])
                count++;
        }
    }

    return count / 3;
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
        int a[6];
        // read array element-by-element
        for (int i = 0; i < 6; ++i) {
            a[i] = i_a.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        int n = 6;
        int result = countTriplets(a, n);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(result);
        /* === Variable Section End === */
    }
}
