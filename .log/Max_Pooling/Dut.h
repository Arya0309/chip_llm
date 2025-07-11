#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_in_h;
  sc_fifo_in<int> i_in_w;
  sc_fifo_in<int> i_pool_h;
  sc_fifo_in<int> i_pool_w;
  sc_fifo_in<int> i_stride_h;
  sc_fifo_in<int> i_stride_w;
  sc_fifo_in<sc_dt::sc_vector<int>> i_input;
  sc_fifo_out<sc_dt::sc_vector<int>> o_output;
  sc_fifo_out<int> o_out_h;
  sc_fifo_out<int> o_out_w;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_