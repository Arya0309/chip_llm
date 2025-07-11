#include "Dut.h"
#include <iostream>

void find_lcm(int a, int b) {
    int max_num, flag = 1;

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
}

SC_MODULE(Dut) {
    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_in<int> i_a;
    sc_in<int> i_b;
    sc_out<int> o_result;

    SC_CTOR(Dut) {
        SC_THREAD(find_lcm_thread);
        sensitive << i_clk.pos();
        dont_initialize();
        reset_signal_is(i_rst, false);
    }

    void find_lcm_thread() {
        wait();
        while (true) {
            int a = i_a.read();
            int b = i_b.read();
            int max_num, flag = 1;

            // Use ternary operator to get the
            // large number
            max_num = (a > b) ? a : b;

            while (flag) {
                // if statement checks max_num is completely
                // divisible by n1 and n2.
                if (max_num % a == 0 && max_num % b == 0) {
                    o_result.write(max_num);
                    break;
                }

                // update by 1 on each iteration
                ++max_num;
            }
        }
    }
};
