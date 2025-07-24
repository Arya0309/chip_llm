#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc.h>

SC_MODULE(Testbench) {
    sc_in_clk i_clk;
    sc_out<bool> o_rst;
    sc_fifo_out<int> o_a;
    sc_fifo_out<int> o_b;
    sc_fifo_in<int> i_result;

    SC_CTOR(Testbench);

    void feed_data();
    void check_results();
};

#endif // TESTBENCH_H_
