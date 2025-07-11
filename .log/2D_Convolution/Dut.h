#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<double> i_input[16];
  sc_fifo_in<double> i_kernel[9];
  sc_fifo_out<double> o_output[16];
  sc_in<int> in_h;
  sc_in<int> in_w;
  sc_in<int> ker_h;
  sc_in<int> ker_w;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void conv2D();
};
#endif
