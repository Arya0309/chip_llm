
#include "Dut.h"

#include <cmath>

bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i <= std::sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

bool isPossible(int N) {
    if (N < 4) return false;           // 2,3 不符合
    if (N % 2 == 1) {                  // 奇數: 只能 2 + (N-2)
        return isPrime(N - 2);
    }
    // 偶數: Goldbach 檢查
    for (int p = 2; p <= N / 2; ++p) {
        if (isPrime(p) && isPrime(N - p))
            return true;
    }
    return false;                      // 找不到質數組合
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
        int N = i_N.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        bool result = isPossible(N);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(result);
        /* === Variable Section End === */
    }
}
