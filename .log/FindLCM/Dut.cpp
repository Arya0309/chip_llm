#include "Dut.h"
#include <iostream>

int main()
{
    int a = 15, b = 20, max_num, flag = 1;

    // Use ternary operator to get the
    // large number
    max_num = (a > b) ? a : b;

    while (flag) {
        // if statement checks max_num is completely
        // divisible by n1 and n2.
        if (max_num % a == 0 && max_num % b == 0) {
            std::cout << "LCM of " << a << " and " << b << " is "
                      << max_num;
            break;
        }

        // update by 1 on each iteration
        ++max_num;
    }
    return 0;
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
        int a = i_a.read();
        int b = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        int max_num = (a > b) ? a : b;
        int flag = 1;
        while (flag) {
            if (max_num % a == 0 && max_num % b == 0) {
                o_result.write(max_num);
                flag = 0;
            } else {
                ++max_num;
            }
        }
        /* === Main function Section End === */

        /* === Variable Section === */
        // No need to write anything here as the result is written directly in the main function section
        /* === Variable Section End === */
    }
}
