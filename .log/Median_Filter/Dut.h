
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_length;
  sc_fifo_in<int> i_window_size;
  sc_fifo_in<sc_dt::sc_lv<32>> i_input;
  sc_fifo_out<sc_dt::sc_lv<32>> o_output;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
