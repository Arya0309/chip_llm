
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
#include <cmath>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

  /* === Variable Section === */
  sc_fifo_in<double> i_a;
  sc_fifo_out<double> o_result;
  /* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_
