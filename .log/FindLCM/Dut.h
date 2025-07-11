#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

SC_MODULE(Dut) {
    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_in<int> i_a;
    sc_in<int> i_b;
    sc_out<int> o_result;

    SC_CTOR(Dut);
};

#endif // DUT_H_
