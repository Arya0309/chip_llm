
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define R 3
#define C 3

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_m;
  sc_fifo_in<int> i_n;
  sc_fifo_in<int> i_mat[R*C];
  sc_fifo_out<int> o_mat[R*C];
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
