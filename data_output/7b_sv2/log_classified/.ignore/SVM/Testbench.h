#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
using namespace sc_core;

class Testbench : public sc_module {
public:
  sc_in_clk i_clk;
  sc_out<bool> o_rst;

  /* === Variable Section === */
  sc_fifo_out<double> o_X;
  sc_fifo_out<int> o_y;
  sc_fifo_out<int> o_n;
  sc_fifo_out<int> o_d;
  sc_fifo_out<double> o_C;
  sc_fifo_out<double> o_lr;
  sc_fifo_out<int> o_epochs;
  sc_fifo_out<double> o_w;
  sc_fifo_out<double> o_b;
  sc_fifo_in<double> i_w;
  /* === Variable Section End === */

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench() = default;

private:

  void do_feed();
  void do_fetch();

};

#endif // TESTBENCH_H_