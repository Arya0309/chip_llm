#include "Dut.h"

std::string addBinary(std::string A, std::string B) {
    if (A.length() > B.length())
        return addBinary(B, A);

    int diff = B.length() - A.length();
    std::string padding;
    for (int i = 0; i < diff; i++)
        padding.push_back('0');

    A = padding + A;
    std::string res;
    char carry = '0';

    for (int i = A.length() - 1; i >= 0; i--) {
        if (A[i] == '1' && B[i] == '1') {
            if (carry == '1')
                res.push_back('1'), carry = '1';
            else
                res.push_back('0'), carry = '1';
        }
        else if (A[i] == '0' && B[i] == '0') {
            if (carry == '1')
                res.push_back('1'), carry = '0';
            else
                res.push_back('0'), carry = '0';
        }
        else if (A[i] != B[i]) {
            if (carry == '1')
                res.push_back('0'), carry = '1';
            else
                res.push_back('1'), carry = '0';
        }
    }

    if (carry == '1')
        res.push_back(carry);
    std::reverse(res.begin(), res.end());

    int index = 0;
    while (index + 1 < res.length() && res[index] == '0')
        index++;
    return (res.substr(index));
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
        std::string A = i_a.read();
        std::string B = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        std::string result = addBinary(A, B);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(result);
        /* === Variable Section End === */
    }
}
