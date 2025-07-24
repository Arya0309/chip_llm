#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    /* === Variable Section === */
    sc_fifo_in<int> i_mat1;
    sc_fifo_in<int> i_mat2;
    sc_fifo_out<int> o_result;
    /* === Variable Section End === */

    SC_HAS_PROCESS(Dut);
    Dut(sc_module_name n);
    ~Dut() = default;

private:
    void do_compute();
};

#endif // DUT_H_
